#include "scene.h"

#include <algorithm>
#include <vector>

Scene::Scene()
    : player_("PLAYER"),
      meleeEnemy_(player_),
      rangedEnemy_(player_),
      playerSpawnX_(player_.getX()),
      playerSpawnY_(player_.getY()),
      shadowSystem_(player_),
      combatSystem_(CHARACTER_WIDTH, CHARACTER_HEIGHT, 16.0F, 8.0F),
      platform_{0.0F, 360.0F, 1200.0F, 40.0F},
      damageTextElapsedTime_(0.0F),
      damageTextX_(0.0F),
      damageTextY_(0.0F),
      displayedDamage_(0.0F),
      attackEffectElapsedTime_(0.0F),
      meleeAttackEffectElapsedTime_(0.0F),
      meleeEnemyExperienceAwarded_(false),
      rangedEnemyExperienceAwarded_(false) {
    // 平台加长时不移动已有敌人；近战兵仍在远程兵左方 100px。
    rangedEnemy_.setPosition(RANGED_ENEMY_SPAWN_X, 0.0F);
    meleeEnemy_.setPosition(rangedEnemy_.getX() - ENEMY_SPACING, 0.0F);

    // 两块小平台分别以敌军身体中心为水平中心，顶面离主地面 60px。
    const float upperPlatformY = platform_.y - UPPER_PLATFORM_HEIGHT;
    const float halfPlatformWidth = UPPER_PLATFORM_WIDTH / 2.0F;
    const float halfCharacterWidth = CHARACTER_WIDTH / 2.0F;
    platformSystem_.addPlatform(Rectangle{
        meleeEnemy_.getX() + halfCharacterWidth - halfPlatformWidth,
        upperPlatformY,
        UPPER_PLATFORM_WIDTH,
        UPPER_PLATFORM_THICKNESS});
    platformSystem_.addPlatform(Rectangle{
        rangedEnemy_.getX() + halfCharacterWidth - halfPlatformWidth,
        upperPlatformY,
        UPPER_PLATFORM_WIDTH,
        UPPER_PLATFORM_THICKNESS});
}

Rectangle Scene::makeCharacterHitbox(
    const Character& character, const Rectangle& platform) {
    return Rectangle{
        character.getX(),
        platform.y - Scene::CHARACTER_HEIGHT - character.getY(),
        Scene::CHARACTER_WIDTH,
        Scene::CHARACTER_HEIGHT};
}

void Scene::update(float deltaTime) {
    // Scene 只读取设备无关状态，不直接判断键盘或手柄的具体按键。
    const PlayerInputState input = playerController_.pollInput();
    const HorizontalInputDirection horizontalDirection =
        PlayerController::resolveHorizontalDirection(input);
    if (!player_.isDodging()) {
        if (horizontalDirection == HorizontalInputDirection::Left) {
            player_.moveLeft(deltaTime);
        } else if (horizontalDirection == HorizontalInputDirection::Right) {
            player_.moveRight(deltaTime);
        }
    }
    if (input.jumpPressed) {
        player_.jump();
    }
    // 防御需要持续按住；闪避期间角色仍可维持防御状态，但敌人不会锁定闪避中的玩家。
    player_.setDefending(input.defendHeld);
    const bool attackPressed = input.meleeAttackPressed && !player_.isDefending();
    if (input.rangedAttackPressed && !player_.isDefending()) {
        player_.rangedAttack();
    }
    if (input.dodgePressed) {
        // 优先使用本帧按下的方向；没有水平输入时沿上次朝向闪避。
        const bool dodgeRight = horizontalDirection == HorizontalInputDirection::Right ||
            (horizontalDirection == HorizontalInputDirection::None && player_.isFacingRight());
        player_.startDodge(dodgeRight);
    }

    // 所有角色共用平台支撑、实体更新、小平台落地和主地面落地顺序。
    const auto updateCharacterPhysics = [&](Character& character) {
        platformSystem_.updateCharacterSupport(character, platform_.y, CHARACTER_WIDTH);
        const float previousHeight = character.getY();
        character.update(deltaTime);
        // 闪避等在 update 内发生的水平位移也可能让角色离开平台。
        platformSystem_.updateCharacterSupport(character, platform_.y, CHARACTER_WIDTH);
        platformSystem_.resolveCharacterLanding(
            character, previousHeight, platform_.y, CHARACTER_WIDTH);
        if (character.getY() <= 0.0F) {
            character.land();
        }
    };
    updateCharacterPhysics(player_);
    updateCharacterPhysics(meleeEnemy_);
    updateCharacterPhysics(rangedEnemy_);

    const float maxPlayerX = platform_.x + platform_.width - CHARACTER_WIDTH;
    player_.setPosition(
        std::clamp(player_.getX(), platform_.x, maxPlayerX), player_.getY());
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
            ? player_.getX() + CHARACTER_WIDTH + PROJECTILE_RADIUS
            : player_.getX() - PROJECTILE_RADIUS;
        projectileSystem_.spawn(ProjectileSystem::SpawnInfo{
            projectileX,
            platform_.y - CHARACTER_HEIGHT / 2.0F - player_.getY(),
            direction,
            PROJECTILE_SPEED,
            PROJECTILE_MAX_DISTANCE,
            PROJECTILE_RADIUS,
            player_.getAttackDamage(),
            Faction::Friendly,
            ORANGE,
            0});
    }

    const CombatSystem::AttackResult enemyMeleeResult = combatSystem_.enemyMeleeAttack(
        meleeEnemy_, player_, shadowSystem_.getShadow(), platform_.y);
    if (enemyMeleeResult.attackPerformed) {
        meleeAttackEffectElapsedTime_ = ATTACK_EFFECT_LIFETIME;
    }

    Character* rangedEnemyTarget = combatSystem_.findNearestEnemyTarget(
        rangedEnemy_, player_, shadowSystem_.getShadow());
    if (rangedEnemyTarget != nullptr) {
        // 朝向、子弹出生侧和飞行方向都使用同一个目标判断结果。
        rangedEnemy_.faceToward(*rangedEnemyTarget);
        if (rangedEnemy_.tryAttack()) {
            const float direction = rangedEnemy_.isFacingRight() ? 1.0F : -1.0F;
            const float projectileX = rangedEnemy_.isFacingRight()
                ? rangedEnemy_.getX() + CHARACTER_WIDTH + PROJECTILE_RADIUS
                : rangedEnemy_.getX() - PROJECTILE_RADIUS;
            projectileSystem_.spawn(ProjectileSystem::SpawnInfo{
                projectileX,
                platform_.y - CHARACTER_HEIGHT / 2.0F - rangedEnemy_.getY(),
                direction,
                PROJECTILE_SPEED,
                PROJECTILE_MAX_DISTANCE,
                PROJECTILE_RADIUS,
                rangedEnemy_.getAttackDamage(),
                Faction::Enemy,
                Color{255, 45, 20, 255},
                0});
        }
    }

    // Scene 只组装本帧目标；弹道筛选阵营、移动、命中和销毁均由系统处理。
    std::vector<ProjectileSystem::Target> projectileTargets;
    ProjectileSystem::Target playerTarget{
        &player_,
        makeCharacterHitbox(player_, platform_),
        player_.isDodging(),
        player_.isDefending(),
        combatSystem_.makePlayerDefenseHitbox(player_, platform_.y),
        [&]() { return player_.blockNextAttack(); }};
    projectileTargets.push_back(playerTarget);
    if (PlayerShadow* shadow = shadowSystem_.getShadow(); shadow != nullptr) {
        projectileTargets.push_back(ProjectileSystem::Target{
            shadow,
            makeCharacterHitbox(*shadow, platform_),
            false, false, Rectangle{}, {}});
    }
    projectileTargets.push_back(ProjectileSystem::Target{
        &meleeEnemy_,
        makeCharacterHitbox(meleeEnemy_, platform_),
        false, false, Rectangle{}, {}});
    projectileTargets.push_back(ProjectileSystem::Target{
        &rangedEnemy_,
        makeCharacterHitbox(rangedEnemy_, platform_),
        false, false, Rectangle{}, {}});

    const std::vector<ProjectileSystem::Impact> projectileImpacts =
        projectileSystem_.update(deltaTime, projectileTargets);
    for (const ProjectileSystem::Impact& impact : projectileImpacts) {
        if (impact.blocked) continue;
        displayedDamage_ = impact.damage;
        damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
        damageTextX_ = impact.textX;
        damageTextY_ = impact.textY;
    }

    if (!player_.isAlive()) {
        // 玩家死亡后立即在出生点回满血复活，并重新开始影子距离累计。
        player_.heal(player_.getMaxHealth());
        player_.setPosition(playerSpawnX_, playerSpawnY_);
        player_.land();
        shadowSystem_.resetPlayerTracking(player_);
    }

    if (attackPressed) {
        const CombatSystem::AttackResult playerAttackResult = combatSystem_.playerMeleeAttack(
            player_, shadowSystem_.getShadow(), {&meleeEnemy_, &rangedEnemy_}, platform_.y);
        if (playerAttackResult.hit) {
            displayedDamage_ = playerAttackResult.damage;
            damageTextElapsedTime_ = DAMAGE_TEXT_LIFETIME;
            damageTextX_ = playerAttackResult.targetHitbox.x +
                playerAttackResult.targetHitbox.width / 2.0F;
            damageTextY_ = playerAttackResult.targetHitbox.y - 20.0F;
        }
    }

    const auto awardEnemyExperience = [&](const Enemy& enemy, bool& experienceAwarded) {
        // 经验标记防止敌人 5 秒尸体期的每一帧都重复加经验。
        if (enemy.isAlive()) {
            experienceAwarded = false;
            return;
        }
        if (!experienceAwarded) {
            // Scene 不计算经验公式，只领取当前兵种自身配置的击杀奖励。
            player_.addExperience(enemy.getExperienceReward());
            experienceAwarded = true;
        }
    };
    awardEnemyExperience(meleeEnemy_, meleeEnemyExperienceAwarded_);
    awardEnemyExperience(rangedEnemy_, rangedEnemyExperienceAwarded_);

    // 影子自身生命周期仍由 ShadowSystem 管理，平台系统只处理实体支撑和落地。
    PlayerShadow* shadowBeforeUpdate = shadowSystem_.getShadow();
    const bool hadShadowBeforeUpdate = shadowBeforeUpdate != nullptr;
    const float previousShadowHeight = hadShadowBeforeUpdate
        ? shadowBeforeUpdate->getY()
        : 0.0F;
    if (shadowBeforeUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(
            *shadowBeforeUpdate, platform_.y, CHARACTER_WIDTH);
    }
    shadowSystem_.update(player_, deltaTime);
    if (PlayerShadow* shadowAfterUpdate = shadowSystem_.getShadow();
        shadowAfterUpdate != nullptr) {
        platformSystem_.updateCharacterSupport(
            *shadowAfterUpdate, platform_.y, CHARACTER_WIDTH);
        const float heightBeforePhysics = hadShadowBeforeUpdate
            ? previousShadowHeight
            : shadowAfterUpdate->getY();
        platformSystem_.resolveCharacterLanding(
            *shadowAfterUpdate, heightBeforePhysics, platform_.y, CHARACTER_WIDTH);
    }
}

void Scene::draw() const {
    ClearBackground(Color{24, 28, 38, 255});

    DrawRectangleRec(platform_, Color{126, 91, 67, 255});
    platformSystem_.draw(Color{126, 91, 67, 255});
    shadowSystem_.draw(platform_.y, CHARACTER_WIDTH, CHARACTER_HEIGHT);

    const auto drawEnemy = [&](const Enemy& enemy, Color eyeColor, bool facingRight) {
        // respawning 控制敌人尸体期的浅色身体和黄色复活倒计时条。
        const float enemyTop = platform_.y - CHARACTER_HEIGHT - enemy.getY();
        const int barX = static_cast<int>(enemy.getX());
        const int barY = static_cast<int>(enemyTop - 10.0F);
        const bool respawning = !enemy.isAlive() || enemy.isRespawning();
        const Color bodyColor = respawning ? Color{235, 170, 170, 255}
                                           : Color{190, 90, 90, 255};
        const float barProgress = respawning
            ? 1.0F - enemy.getRespawnProgress()
            : enemy.getHealth() / enemy.getMaxHealth();

        DrawRectangle(static_cast<int>(enemy.getX()), static_cast<int>(enemyTop),
                      static_cast<int>(CHARACTER_WIDTH), static_cast<int>(CHARACTER_HEIGHT), bodyColor);
        DrawRectangle(barX, barY, static_cast<int>(CHARACTER_WIDTH), 5, DARKGRAY);
        DrawRectangle(barX, barY,
                      static_cast<int>(CHARACTER_WIDTH * std::clamp(barProgress, 0.0F, 1.0F)),
                      5, respawning ? YELLOW : RED);
        const float eyeX = facingRight
            ? enemy.getX() + CHARACTER_WIDTH - 8.0F
            : enemy.getX() + 8.0F;
        DrawCircle(static_cast<int>(eyeX),
                   static_cast<int>(enemyTop + 14.0F), 3.0F, eyeColor);
    };
    drawEnemy(meleeEnemy_, BLUE, meleeEnemy_.isFacingRight());
    drawEnemy(rangedEnemy_, GREEN, rangedEnemy_.isFacingRight());
    DrawRectangle(
        static_cast<int>(player_.getX()),
        static_cast<int>(platform_.y - CHARACTER_HEIGHT - player_.getY()),
        static_cast<int>(CHARACTER_WIDTH),
        static_cast<int>(CHARACTER_HEIGHT),
        Color{72, 183, 255, 255});
    const float playerTop = platform_.y - CHARACTER_HEIGHT - player_.getY();
    if (player_.isDodgeCoolingDown()) {
        // 背向由当前朝向决定：右向时条在角色左侧，左向时条在角色右侧。
        const float cooldownProgress = std::clamp(player_.getDodgeCooldownProgress(), 0.0F, 1.0F);
        const int barX = static_cast<int>(player_.isFacingRight()
            ? player_.getX() - 6.0F
            : player_.getX() + CHARACTER_WIDTH + 2.0F);
        const int barY = static_cast<int>(playerTop);
        const int barHeight = static_cast<int>(CHARACTER_HEIGHT);
        const int progressHeight = static_cast<int>(barHeight * cooldownProgress);
        DrawRectangle(barX, barY, 4, barHeight, DARKGRAY);
        DrawRectangle(barX, barY + barHeight - progressHeight, 4, progressHeight, GREEN);
    }
    const float playerHealthRatio = player_.getHealth() / player_.getMaxHealth();
    DrawRectangle(static_cast<int>(player_.getX()), static_cast<int>(playerTop - 10.0F),
                  static_cast<int>(CHARACTER_WIDTH), 5, DARKGRAY);
    DrawRectangle(static_cast<int>(player_.getX()), static_cast<int>(playerTop - 10.0F),
                  static_cast<int>(CHARACTER_WIDTH * std::clamp(playerHealthRatio, 0.0F, 1.0F)),
                  5, GREEN);
    const float eyeX = player_.isFacingRight()
        ? player_.getX() + CHARACTER_WIDTH - 8.0F
        : player_.getX() + 8.0F;
    DrawCircle(static_cast<int>(eyeX), static_cast<int>(playerTop + 14.0F), 3.0F, BLACK);
    if (player_.isDefending() || attackEffectElapsedTime_ > 0.0F) {
        const float bladeBaseX = player_.isFacingRight()
            ? player_.getX() + CHARACTER_WIDTH
            : player_.getX();
        const Vector2 bladeTop{bladeBaseX, playerTop};
        const Vector2 bladeBottom{bladeBaseX, playerTop + CHARACTER_HEIGHT};
        const float bladeLength = player_.isDefending()
            ? combatSystem_.getDefenseRange()
            : combatSystem_.getAttackRange();
        const Vector2 bladeTip{
            player_.isFacingRight() ? bladeBaseX + bladeLength : bladeBaseX - bladeLength,
            playerTop + CHARACTER_HEIGHT / 2.0F};
        const Color bladeColor = player_.isDefending() ? Color{255, 222, 173, 255} : WHITE;
        if (player_.isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, bladeColor);
        } else {
            // 左向时交换底边顶点，保持与右向一致的三角形顶点绕序。
            DrawTriangle(bladeBottom, bladeTop, bladeTip, bladeColor);
        }
    }
    if (meleeAttackEffectElapsedTime_ > 0.0F) {
        const float enemyTop = platform_.y - CHARACTER_HEIGHT - meleeEnemy_.getY();
        const float bladeBaseX = meleeEnemy_.isFacingRight()
            ? meleeEnemy_.getX() + CHARACTER_WIDTH
            : meleeEnemy_.getX();
        const Vector2 bladeTop{bladeBaseX, enemyTop};
        const Vector2 bladeBottom{bladeBaseX, enemyTop + CHARACTER_HEIGHT};
        const Vector2 bladeTip{
            meleeEnemy_.isFacingRight()
                ? bladeBaseX + combatSystem_.getAttackRange()
                : bladeBaseX - combatSystem_.getAttackRange(),
            enemyTop + CHARACTER_HEIGHT / 2.0F};
        if (meleeEnemy_.isFacingRight()) {
            DrawTriangle(bladeTop, bladeBottom, bladeTip, Color{255, 182, 193, 255});
        } else {
            DrawTriangle(bladeBottom, bladeTop, bladeTip, Color{255, 182, 193, 255});
        }
    }
    projectileSystem_.draw();
    if (damageTextElapsedTime_ > 0.0F) {
        DrawText(TextFormat("%.0f", displayedDamage_), static_cast<int>(damageTextX_),
                 static_cast<int>(damageTextY_), 20, YELLOW);
    }
    DrawText("A/D or arrow keys: move    Space: jump    J: melee    K: ranged    U: defend    L: dodge", 20, 20, 20,
             RAYWHITE);
    DrawText(TextFormat("Level: %d", player_.getLevel()), 20, 50, 20, RAYWHITE);
    DrawText(TextFormat("Exp: %d / %d", player_.getExperience(),
                        player_.getExperienceThreshold()),
             20, 76, 20, RAYWHITE);
}
