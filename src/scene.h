#pragma once

#include "melee_enemy.h"
#include "player.h"
#include "player_shadow.h"
#include "ranged_enemy.h"
#include "raylib.h"

#include <optional>
#include <vector>

class Scene {
public:
    Scene();

    void update(float deltaTime);
    void draw() const;

private:
    struct Projectile {
        float x;
        float y;
        float direction;
        float travelledDistance;
        float damage;
        Faction faction;
        bool hitPlayer;
        bool hitShadow;
        bool hitMeleeEnemy;
        bool hitRangedEnemy;
    };

    Player player_;
    MeleeEnemy meleeEnemy_;
    RangedEnemy rangedEnemy_;
    float playerSpawnX_;
    float playerSpawnY_;
    // 同时最多保留一个影子；空值表示正等待玩家横向移动满 300px。
    std::optional<PlayerShadow> playerShadow_;
    Rectangle platform_;
    // 影子消失时记录玩家当前位置，下一次影子将在该记录点生成。
    float recordedPlayerX_;
    float recordedPlayerY_;
    float previousPlayerX_;
    // 等待影子生成时累计的横向实际移动距离；折返移动同样计入。
    float accumulatedPlayerHorizontalDistance_;
    // 用于影子 10 秒生命周期和命中伤害数字 1 秒显示时间。
    float shadowElapsedTime_;
    float damageTextElapsedTime_;
    float damageTextX_;
    float damageTextY_;
    float displayedDamage_;
    float attackEffectElapsedTime_;
    float meleeAttackEffectElapsedTime_;
    bool meleeEnemyExperienceAwarded_;
    bool rangedEnemyExperienceAwarded_;
    std::vector<Projectile> projectiles_;

    static constexpr float PLAYER_WIDTH = 32.0F;
    static constexpr float PLAYER_HEIGHT = 48.0F;
    static constexpr float SHADOW_DISTANCE = 300.0F; // 仅计算 x 轴位移
    static constexpr float SHADOW_LIFETIME = 10.0F; // 影子最长存在时间
    static constexpr float DAMAGE_TEXT_LIFETIME = 1.0F;
    static constexpr float ATTACK_EFFECT_LIFETIME = 0.15F;
    static constexpr float ATTACK_RANGE = 16.0F;
    static constexpr float DEFENSE_RANGE = 8.0F;
    static constexpr float PROJECTILE_RADIUS = 4.0F;
    static constexpr float PROJECTILE_SPEED = 300.0F;
    static constexpr float PROJECTILE_MAX_DISTANCE = 500.0F;
    static constexpr float ENEMY_SPACING = 100.0F;
    static constexpr float ENEMY_DETECTION_RANGE = 300.0F;
    static constexpr int ENEMY_EXPERIENCE_REWARD = 5;
};
