#include "scene.h"

#include <algorithm>
#include <cmath>

Scene::Scene()
    : player_("PLAYER"),
      meleeEnemy_(player_),
      rangedEnemy_(player_),
      playerSpawnX_(player_.getX()),
      playerSpawnY_(player_.getY()),
      platform_{0.0F, 360.0F, 800.0F, 40.0F},
      recordedPlayerX_(player_.getX()),
      recordedPlayerY_(player_.getY()),
      previousPlayerX_(player_.getX()),
      accumulatedPlayerHorizontalDistance_(0.0F),
      shadowElapsedTime_(0.0F),
      damageTextElapsedTime_(0.0F),
      damageTextX_(0.0F),
      damageTextY_(0.0F),
      displayedDamage_(0.0F),
      attackEffectElapsedTime_(0.0F),
      meleeAttackEffectElapsedTime_(0.0F) {
    rangedEnemy_.setPosition(platform_.x + platform_.width - PLAYER_WIDTH, 0.0F);
    meleeEnemy_.setPosition(rangedEnemy_.getX() - ENEMY_SPACING, 0.0F);
}

void Scene::update(float deltaTime) {
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        player_.moveLeft(deltaTime);
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        player_.moveRight(deltaTime);
    }
    if (IsKeyPressed(KEY_SPACE)) {
        player_.jump();
    }
    player_.setDefending(IsKeyDown(KEY_U));
    const bool attackPressed = IsKeyPressed(KEY_J) && !player_.isDefending();
    if (IsKeyPressed(KEY_K) && !player_.isDefending()) {
        player_.rangedAttack();
    }

    player_.update(deltaTime);

    if (player_.getY() <= 0.0F) {
        player_.land();
    }
    meleeEnemy_.update(deltaTime);
    rangedEnemy_.update(deltaTime);
    if (meleeEnemy_.getY() <= 0.0F) {
        meleeEnemy_.land();
    }
    if (rangedEnemy_.getY() <= 0.0F) {
        rangedEnemy_.land();
    }

    const float maxPlayerX = platform_.x + platform_.width - PLAYER_WIDTH;
    player_.setPosition(
        std::clamp(player_.getX(), platform_.x, maxPlayerX), player_.getY());
    const float frameHorizontalDistance = std::abs(player_.getX() - previousPlayerX_);
    previousPlayerX_ = player_.getX();

    // 伤害数字只保留一秒，与影子是否已经消失无关。
    if (damageTextElapsedTime_ > 0.0F) {
        damageTextElapsedTime_ = std::max(0.0F, damageTextElapsedTime_ - deltaTime);
    }
    if (attackEffectElapsedTime_ > 0.0F) {
        attackEffectElapsedTime_ =
            std::max(0.0F, attackEffectElapsedTime_ - deltaTime);
    }
    if (meleeAttackEffectElapsedTime_ > 0.0F) {
        meleeAttackEffectElapsedTime_ =
            std::max(0.0F, meleeAttackEffectElapsedTime_ - deltaTime);
    }
    if (attackPressed) {
        attackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
    }

    if (player_.consumeRangedAttackRequest()) {
        const float direction = player_.isFacingRight() ? 1.0F : -1.0F;
        const float projectileX = player_.isFacingRight()
            ? player_.getX() + PLAYER_WIDTH + PROJECTILE_RADIUS
            : player_.getX() - PROJECTILE_RADIUS;
        projectiles_.push_back(Projectile{
            projectileX,
            platform_.y - PLAYER_HEIGHT / 2.0F - player_.getY(),
            direction,
            0.0F,
            player_.getAttackDamage(),
            Faction::Friendly,
            false, false, false, false});
    }

    const auto findNearestEnemyTarget = [&](const Enemy& enemy) -> Character* {
        Character* target = nullptr;
        float nearestDistance = ENEMY_DETECTION_RANGE;

        const auto considerTarget = [&](Character& candidate) {
            if (!candidate.isAlive() || candidate.getFaction() == enemy.getFaction()) return;

            const float distance = std::abs(candidate.getX() - enemy.getX());
            if (distance <= nearestDistance) {
                nearestDistance = distance;
                target = &candidate;
            }
        };

        considerTarget(player_);
        if (playerShadow_.has_value()) {
            considerTarget(*playerShadow_);
        }
        return target;
    };

    // 近战兵不依赖索敌：只要存活且冷却结束就挥刀；命中范围内的非同阵营目标才受伤。
    if (meleeEnemy_.tryAttack()) {
        meleeAttackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
        Character* target = findNearestEnemyTarget(meleeEnemy_);
        if (target != nullptr) {
            const Rectangle meleeAttackHitbox{
                meleeEnemy_.getX() - ATTACK_RANGE,
                platform_.y - PLAYER_HEIGHT - meleeEnemy_.getY(),
                ATTACK_RANGE, PLAYER_HEIGHT};
            const Rectangle targetHitbox{
                target->getX(), platform_.y - PLAYER_HEIGHT - target->getY(),
                PLAYER_WIDTH, PLAYER_HEIGHT};
            const Rectangle playerDefenseHitbox{
                player_.isFacingRight() ? player_.getX() + PLAYER_WIDTH
                                        : player_.getX() - DEFENSE_RANGE,
                platform_.y - PLAYER_HEIGHT - player_.getY(), DEFENSE_RANGE, PLAYER_HEIGHT};
            const bool isBlocked = target == &player_ && player_.isDefending() &&
                CheckCollisionRecs(meleeAttackHitbox, playerDefenseHitbox) &&
                player_.blockNextAttack();
            if (CheckCollisionRecs(meleeAttackHitbox, targetHitbox) && !isBlocked) {
                target->takeDamage(meleeEnemy_.getAttackDamage());
            }
        }
    }

    if (Character* target = findNearestEnemyTarget(rangedEnemy_);
        target != nullptr && rangedEnemy_.tryAttack()) {
        projectiles_.push_back(Projectile{
            rangedEnemy_.getX() - PROJECTILE_RADIUS,
            platform_.y - PLAYER_HEIGHT / 2.0F - rangedEnemy_.getY(),
            -1.0F,
            0.0F,
            rangedEnemy_.getAttackDamage(),
            Faction::Enemy,
            false, false, false, false});
    }

    for (auto iterator = projectiles_.begin(); iterator != projectiles_.end();) {
        iterator->x += iterator->direction * PROJECTILE_SPEED * deltaTime;
        iterator->travelledDistance += PROJECTILE_SPEED * deltaTime;
        bool hitDamageableTarget = false;
        const Rectangle playerDefenseHitbox{
            player_.isFacingRight() ? player_.getX() + PLAYER_WIDTH
                                    : player_.getX() - DEFENSE_RANGE,
            platform_.y - PLAYER_HEIGHT - player_.getY(), DEFENSE_RANGE, PLAYER_HEIGHT};

        const auto damageTarget = [&](Character& target, const Rectangle& hitbox,
                                      bool& hasBeenHit) {
            if (hitDamageableTarget || hasBeenHit || !target.isAlive() ||
                target.getFaction() == iterator->faction) return;
            if (!CheckCollisionCircleRec(
                    Vector2{iterator->x, iterator->y}, PROJECTILE_RADIUS, hitbox)) return;

            if (&target == &player_ && player_.isDefending() &&
                CheckCollisionCircleRec(Vector2{iterator->x, iterator->y}, PROJECTILE_RADIUS,
                                        playerDefenseHitbox) &&
                player_.blockNextAttack()) {
                hasBeenHit = true;
                hitDamageableTarget = true;
                return;
            }
            target.takeDamage(iterator->damage);
            hasBeenHit = true;
            hitDamageableTarget = true;
            displayedDamage_ = iterator->damage;
            damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
            damageTextX_ = hitbox.x + hitbox.width / 2.0F;
            damageTextY_ = hitbox.y - 20.0F;
        };

        damageTarget(player_, Rectangle{
            player_.getX(), platform_.y - PLAYER_HEIGHT - player_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT}, iterator->hitPlayer);
        if (playerShadow_.has_value()) {
            const Rectangle shadowHitbox{
                playerShadow_->getX(),
                platform_.y - PLAYER_HEIGHT - playerShadow_->getY(),
                PLAYER_WIDTH, PLAYER_HEIGHT};
            damageTarget(*playerShadow_, shadowHitbox, iterator->hitShadow);
        }
        const Rectangle meleeEnemyHitbox{
            meleeEnemy_.getX(), platform_.y - PLAYER_HEIGHT - meleeEnemy_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        const Rectangle rangedEnemyHitbox{
            rangedEnemy_.getX(), platform_.y - PLAYER_HEIGHT - rangedEnemy_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        damageTarget(meleeEnemy_, meleeEnemyHitbox, iterator->hitMeleeEnemy);
        damageTarget(rangedEnemy_, rangedEnemyHitbox, iterator->hitRangedEnemy);

        // 命中第一个可伤害的非同阵营单位后销毁；同阵营单位直接穿过。
        if (hitDamageableTarget || iterator->travelledDistance >= PROJECTILE_MAX_DISTANCE) {
            iterator = projectiles_.erase(iterator);
        } else {
            ++iterator;
        }
    }

    if (!player_.isAlive()) {
        // 玩家死亡后立即在出生点回满血复活，并重新开始影子距离累计。
        player_.heal(player_.getMaxHealth());
        player_.setPosition(playerSpawnX_, playerSpawnY_);
        player_.land();
        previousPlayerX_ = playerSpawnX_;
        accumulatedPlayerHorizontalDistance_ = 0.0F;
    }

    if (!playerShadow_.has_value() && attackPressed) {
        const float playerTop = platform_.y - PLAYER_HEIGHT - player_.getY();
        const float attackX = player_.isFacingRight()
            ? player_.getX() + PLAYER_WIDTH
            : player_.getX() - ATTACK_RANGE;
        const Rectangle playerAttackHitbox{
            attackX, playerTop, ATTACK_RANGE, PLAYER_HEIGHT};
        const Rectangle meleeEnemyHitbox{
            meleeEnemy_.getX(), platform_.y - PLAYER_HEIGHT - meleeEnemy_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        const Rectangle rangedEnemyHitbox{
            rangedEnemy_.getX(), platform_.y - PLAYER_HEIGHT - rangedEnemy_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};

        Character* hitTarget = nullptr;
        Rectangle hitbox{};
        if (CheckCollisionRecs(playerAttackHitbox, meleeEnemyHitbox)) {
            hitTarget = &meleeEnemy_;
            hitbox = meleeEnemyHitbox;
        } else if (CheckCollisionRecs(playerAttackHitbox, rangedEnemyHitbox)) {
            hitTarget = &rangedEnemy_;
            hitbox = rangedEnemyHitbox;
        }

        if (hitTarget != nullptr) {
            player_.attack(*hitTarget);
            displayedDamage_ = player_.getAttackDamage();
            damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
            damageTextX_ = hitbox.x + hitbox.width / 2.0F;
            damageTextY_ = hitbox.y - 20.0F;
        }
    }

    if (playerShadow_.has_value()) {
        // 影子是实体：空中时按 Character 的重力规则下落。
        playerShadow_->update(deltaTime);
        if (playerShadow_->getY() <= 0.0F) {
            playerShadow_->land();
        }

        const float playerTop = platform_.y - PLAYER_HEIGHT - player_.getY();
        const float attackX = player_.isFacingRight()
            ? player_.getX() + PLAYER_WIDTH
            : player_.getX() - ATTACK_RANGE;
        const Rectangle playerAttackHitbox{
            attackX, playerTop, ATTACK_RANGE, PLAYER_HEIGHT};
        const Rectangle shadowHitbox{
            playerShadow_->getX(),
            platform_.y - PLAYER_HEIGHT - playerShadow_->getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        const Rectangle meleeEnemyHitbox{
            meleeEnemy_.getX(), platform_.y - PLAYER_HEIGHT - meleeEnemy_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        const Rectangle rangedEnemyHitbox{
            rangedEnemy_.getX(), platform_.y - PLAYER_HEIGHT - rangedEnemy_.getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        Character* hitTarget = nullptr;
        Rectangle hitbox{};
        if (CheckCollisionRecs(playerAttackHitbox, shadowHitbox)) {
            hitTarget = &*playerShadow_;
            hitbox = shadowHitbox;
        } else if (CheckCollisionRecs(playerAttackHitbox, meleeEnemyHitbox)) {
            hitTarget = &meleeEnemy_;
            hitbox = meleeEnemyHitbox;
        } else if (CheckCollisionRecs(playerAttackHitbox, rangedEnemyHitbox)) {
            hitTarget = &rangedEnemy_;
            hitbox = rangedEnemyHitbox;
        }
        // J 只命中朝向正前方 16px 的攻击框，属于近战攻击。
        if (attackPressed && hitTarget != nullptr) {
            player_.attack(*hitTarget);
            displayedDamage_ = player_.getAttackDamage();
            damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
            damageTextX_ = hitbox.x + hitbox.width / 2.0F;
            damageTextY_ = hitbox.y - 20.0F;
        }

        shadowElapsedTime_ += deltaTime;
        // 倒计时结束或生命归零都会移除影子，并以当前玩家位置开始下一轮。
        if (shadowElapsedTime_ >= SHADOW_LIFETIME || !playerShadow_->isAlive()) {
            playerShadow_.reset();
            recordedPlayerX_ = player_.getX();
            recordedPlayerY_ = player_.getY();
            accumulatedPlayerHorizontalDistance_ = 0.0F;
            shadowElapsedTime_ = 0.0F;
        }
        return;
    }

    // 仅在没有影子时累加横向位移，因此折返和跳跃中的横移都会计数。
    accumulatedPlayerHorizontalDistance_ += frameHorizontalDistance;
    if (accumulatedPlayerHorizontalDistance_ >= SHADOW_DISTANCE) {
        playerShadow_.emplace(player_, recordedPlayerX_, recordedPlayerY_);
        shadowElapsedTime_ = 0.0F;
    }
}

void Scene::draw() const {
    ClearBackground(Color{24, 28, 38, 255});

    DrawRectangleRec(platform_, Color{126, 91, 67, 255});
    if (playerShadow_.has_value()) {
        const Rectangle shadowHitbox{
            playerShadow_->getX(),
            platform_.y - PLAYER_HEIGHT - playerShadow_->getY(),
            PLAYER_WIDTH, PLAYER_HEIGHT};
        // 红色生命条、绿色倒计时条和红框都以同一个碰撞箱为基准绘制。
        const float healthRatio = playerShadow_->getHealth() / playerShadow_->getMaxHealth();
        const float timeRatio = 1.0F - shadowElapsedTime_ / SHADOW_LIFETIME;
        const int barX = static_cast<int>(shadowHitbox.x);
        const int healthBarY = static_cast<int>(shadowHitbox.y - 18.0F);
        const int timeBarY = healthBarY - 7;
        const int barWidth = static_cast<int>(PLAYER_WIDTH);

        DrawRectangle(
            static_cast<int>(shadowHitbox.x), static_cast<int>(shadowHitbox.y),
            static_cast<int>(PLAYER_WIDTH),
            static_cast<int>(PLAYER_HEIGHT),
            Color{72, 183, 255, 100});
        DrawRectangleRec(shadowHitbox, Color{255, 100, 100, 60});
        DrawRectangleLinesEx(shadowHitbox, 2.0F, RED);
        DrawRectangle(barX, healthBarY, barWidth, 5, DARKGRAY);
        DrawRectangle(barX, healthBarY,
                      static_cast<int>(barWidth * std::clamp(healthRatio, 0.0F, 1.0F)),
                      5, RED);
        DrawRectangle(barX, timeBarY, barWidth, 4, DARKGRAY);
        DrawRectangle(barX, timeBarY,
                      static_cast<int>(barWidth * std::clamp(timeRatio, 0.0F, 1.0F)),
                      4, GREEN);
    } else {
        // 记录点模型只提示下一次影子的生成位置，不参与碰撞或受击。
        DrawRectangle(
            static_cast<int>(recordedPlayerX_),
            static_cast<int>(platform_.y - PLAYER_HEIGHT - recordedPlayerY_),
            static_cast<int>(PLAYER_WIDTH),
            static_cast<int>(PLAYER_HEIGHT),
            Color{120, 170, 255, 80});
    }

    const auto drawEnemy = [&](const Enemy& enemy, Color eyeColor) {
        const float enemyTop = platform_.y - PLAYER_HEIGHT - enemy.getY();
        const int barX = static_cast<int>(enemy.getX());
        const int barY = static_cast<int>(enemyTop - 10.0F);
        const bool respawning = !enemy.isAlive() || enemy.isRespawning();
        const Color bodyColor = respawning ? Color{235, 170, 170, 255}
                                           : Color{190, 90, 90, 255};
        const float barProgress = respawning
            ? 1.0F - enemy.getRespawnProgress()
            : enemy.getHealth() / enemy.getMaxHealth();

        DrawRectangle(static_cast<int>(enemy.getX()), static_cast<int>(enemyTop),
                      static_cast<int>(PLAYER_WIDTH), static_cast<int>(PLAYER_HEIGHT), bodyColor);
        DrawRectangle(barX, barY, static_cast<int>(PLAYER_WIDTH), 5, DARKGRAY);
        DrawRectangle(barX, barY,
                      static_cast<int>(PLAYER_WIDTH * std::clamp(barProgress, 0.0F, 1.0F)),
                      5, respawning ? YELLOW : RED);
        DrawCircle(static_cast<int>(enemy.getX() + 8.0F),
                   static_cast<int>(enemyTop + 14.0F), 3.0F, eyeColor);
    };
    drawEnemy(meleeEnemy_, BLUE);
    drawEnemy(rangedEnemy_, GREEN);
    DrawRectangle(
        static_cast<int>(player_.getX()),
        static_cast<int>(platform_.y - PLAYER_HEIGHT - player_.getY()),
        static_cast<int>(PLAYER_WIDTH),
        static_cast<int>(PLAYER_HEIGHT),
        Color{72, 183, 255, 255});
    const float playerTop = platform_.y - PLAYER_HEIGHT - player_.getY();
    const float playerHealthRatio = player_.getHealth() / player_.getMaxHealth();
    DrawRectangle(static_cast<int>(player_.getX()), static_cast<int>(playerTop - 10.0F),
                  static_cast<int>(PLAYER_WIDTH), 5, DARKGRAY);
    DrawRectangle(static_cast<int>(player_.getX()), static_cast<int>(playerTop - 10.0F),
                  static_cast<int>(PLAYER_WIDTH * std::clamp(playerHealthRatio, 0.0F, 1.0F)),
                  5, GREEN);
    const float eyeX = player_.isFacingRight()
        ? player_.getX() + PLAYER_WIDTH - 8.0F
        : player_.getX() + 8.0F;
    DrawCircle(static_cast<int>(eyeX), static_cast<int>(playerTop + 14.0F), 3.0F, BLACK);
    if (player_.isDefending() || attackEffectElapsedTime_ > 0.0F) {
        const float bladeBaseX = player_.isFacingRight()
            ? player_.getX() + PLAYER_WIDTH
            : player_.getX();
        const Vector2 bladeTop{bladeBaseX, playerTop};
        const Vector2 bladeBottom{bladeBaseX, playerTop + PLAYER_HEIGHT};
        const float bladeLength = player_.isDefending() ? DEFENSE_RANGE : ATTACK_RANGE;
        const Vector2 bladeTip{
            player_.isFacingRight() ? bladeBaseX + bladeLength : bladeBaseX - bladeLength,
            playerTop + PLAYER_HEIGHT / 2.0F};
        const Color bladeColor = player_.isDefending() ? Color{255, 222, 173, 255} : WHITE;
        if (player_.isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, bladeColor);
        } else {
            // 左向时交换底边顶点，保持与右向一致的三角形顶点绕序。
            DrawTriangle(bladeBottom, bladeTop, bladeTip, bladeColor);
        }
    }
    if (meleeAttackEffectElapsedTime_ > 0.0F) {
        const float enemyTop = platform_.y - PLAYER_HEIGHT - meleeEnemy_.getY();
        const float bladeBaseX = meleeEnemy_.getX();
        const Vector2 bladeTop{bladeBaseX, enemyTop};
        const Vector2 bladeBottom{bladeBaseX, enemyTop + PLAYER_HEIGHT};
        const Vector2 bladeTip{bladeBaseX - ATTACK_RANGE, enemyTop + PLAYER_HEIGHT / 2.0F};
        DrawTriangle(bladeBottom, bladeTop, bladeTip, Color{255, 182, 193, 255});
    }
    for (const Projectile& projectile : projectiles_) {
        DrawCircle(static_cast<int>(projectile.x), static_cast<int>(projectile.y),
                   PROJECTILE_RADIUS,
                   projectile.faction == Faction::Enemy
                       ? Color{255, 45, 20, 255}
                       : ORANGE);
    }
    if (damageTextElapsedTime_ > 0.0F) {
        DrawText(TextFormat("%.0f", displayedDamage_), static_cast<int>(damageTextX_),
                 static_cast<int>(damageTextY_), 20, YELLOW);
    }
    DrawText("A/D or arrow keys: move    Space: jump    J: melee    K: ranged    U: defend", 20, 20, 20,
             RAYWHITE);
}
