#include "scene.h"

#include <algorithm>
#include <cmath>

Scene::Scene()
    : player_("PLAYER"),
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
      attackEffectElapsedTime_(0.0F) {
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
    const bool attackPressed = IsKeyPressed(KEY_J);
    if (IsKeyPressed(KEY_K)) {
        player_.rangedAttack();
    }

    player_.update(deltaTime);

    if (player_.getY() <= 0.0F) {
        player_.land();
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
            0.0F});
    }

    for (auto iterator = projectiles_.begin(); iterator != projectiles_.end();) {
        iterator->x += iterator->direction * PROJECTILE_SPEED * deltaTime;
        iterator->travelledDistance += PROJECTILE_SPEED * deltaTime;

        bool hitShadow = false;
        if (playerShadow_.has_value()) {
            const Rectangle shadowHitbox{
                playerShadow_->getX(),
                platform_.y - PLAYER_HEIGHT - playerShadow_->getY(),
                PLAYER_WIDTH, PLAYER_HEIGHT};
            hitShadow = CheckCollisionCircleRec(
                Vector2{iterator->x, iterator->y}, PROJECTILE_RADIUS, shadowHitbox);
            if (hitShadow) {
                player_.attack(*playerShadow_);
                displayedDamage_ = player_.getAttackDamage();
                damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
                damageTextX_ = shadowHitbox.x + shadowHitbox.width / 2.0F;
                damageTextY_ = shadowHitbox.y - 20.0F;
            }
        }

        if (hitShadow || iterator->travelledDistance >= PROJECTILE_MAX_DISTANCE) {
            iterator = projectiles_.erase(iterator);
        } else {
            ++iterator;
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
        // J 只命中朝向正前方 16px 的攻击框，属于近战攻击。
        if (attackPressed && CheckCollisionRecs(playerAttackHitbox, shadowHitbox)) {
            player_.attack(*playerShadow_);
            displayedDamage_ = player_.getAttackDamage();
            damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
            damageTextX_ = shadowHitbox.x + shadowHitbox.width / 2.0F;
            damageTextY_ = shadowHitbox.y - 20.0F;
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
    DrawRectangle(
        static_cast<int>(player_.getX()),
        static_cast<int>(platform_.y - PLAYER_HEIGHT - player_.getY()),
        static_cast<int>(PLAYER_WIDTH),
        static_cast<int>(PLAYER_HEIGHT),
        Color{72, 183, 255, 255});
    const float playerTop = platform_.y - PLAYER_HEIGHT - player_.getY();
    const float eyeX = player_.isFacingRight()
        ? player_.getX() + PLAYER_WIDTH - 8.0F
        : player_.getX() + 8.0F;
    DrawCircle(static_cast<int>(eyeX), static_cast<int>(playerTop + 14.0F), 3.0F, BLACK);
    if (attackEffectElapsedTime_ > 0.0F) {
        const float bladeBaseX = player_.isFacingRight()
            ? player_.getX() + PLAYER_WIDTH
            : player_.getX();
        const Vector2 bladeTop{bladeBaseX, playerTop};
        const Vector2 bladeBottom{bladeBaseX, playerTop + PLAYER_HEIGHT};
        const Vector2 bladeTip{
            player_.isFacingRight() ? bladeBaseX + ATTACK_RANGE : bladeBaseX - ATTACK_RANGE,
            playerTop + PLAYER_HEIGHT / 2.0F};
        if (player_.isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, WHITE);
        } else {
            // 左向时交换底边顶点，保持与右向一致的三角形顶点绕序。
            DrawTriangle(bladeBottom, bladeTop, bladeTip, WHITE);
        }
    }
    for (const Projectile& projectile : projectiles_) {
        DrawCircle(static_cast<int>(projectile.x), static_cast<int>(projectile.y),
                   PROJECTILE_RADIUS, ORANGE);
    }
    if (damageTextElapsedTime_ > 0.0F) {
        DrawText(TextFormat("%.0f", displayedDamage_), static_cast<int>(damageTextX_),
                 static_cast<int>(damageTextY_), 20, YELLOW);
    }
    DrawText("A/D or arrow keys: move    Space: jump    J: melee    K: ranged", 20, 20, 20,
             RAYWHITE);
}
