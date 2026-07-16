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
        float x; // 子弹圆心的屏幕 x 坐标
        float y; // 子弹圆心的屏幕 y 坐标
        float direction; // 水平飞行方向：1 向右，-1 向左
        float travelledDistance; // 已累计飞行距离，用于最大距离销毁
        float damage; // 命中时造成的伤害数值
        Faction faction; // 发射者阵营，用于排除同阵营目标
        bool hitPlayer; // 预留：该子弹是否已命中过玩家
        bool hitShadow; // 预留：该子弹是否已命中过影子
        bool hitMeleeEnemy; // 预留：该子弹是否已命中过近战兵
        bool hitRangedEnemy; // 预留：该子弹是否已命中过远程兵
    };

    Player player_; // 受键盘控制的主角实体
    MeleeEnemy meleeEnemy_; // 平台右侧生成的近战敌军
    RangedEnemy rangedEnemy_; // 平台右侧生成的远程敌军
    float playerSpawnX_; // 玩家死亡后复活使用的出生点 x 坐标
    float playerSpawnY_; // 玩家死亡后复活使用的出生点 y 坐标
    // 同时最多保留一个影子；空值表示正等待玩家横向移动满 300px。
    std::optional<PlayerShadow> playerShadow_;
    Rectangle platform_; // 平台的位置与尺寸，同时定义地面高度
    // 影子消失时记录玩家当前位置，下一次影子将在该记录点生成。
    float recordedPlayerX_;
    float recordedPlayerY_;
    float previousPlayerX_;
    // 等待影子生成时累计的横向实际移动距离；折返移动同样计入。
    float accumulatedPlayerHorizontalDistance_;
    // 用于影子 10 秒生命周期和命中伤害数字 1 秒显示时间。
    float shadowElapsedTime_; // 当前影子已经存在的时间，单位：秒
    float damageTextElapsedTime_; // 最近一次伤害数字还应显示多久，单位：秒
    float damageTextX_; // 最近一次伤害数字的绘制 x 坐标
    float damageTextY_; // 最近一次伤害数字的绘制 y 坐标
    float displayedDamage_; // 最近一次命中的伤害数值
    float attackEffectElapsedTime_; // 玩家白色近战特效的剩余显示时间
    float meleeAttackEffectElapsedTime_; // 敌军粉色近战特效的剩余显示时间
    bool meleeEnemyExperienceAwarded_; // 本次近战兵死亡是否已经奖励过经验
    bool rangedEnemyExperienceAwarded_; // 本次远程兵死亡是否已经奖励过经验
    std::vector<Projectile> projectiles_; // 当前场景内所有仍在飞行的子弹

    static constexpr float PLAYER_WIDTH = 32.0F; // 所有角色共用的碰撞箱宽度
    static constexpr float PLAYER_HEIGHT = 48.0F; // 所有角色共用的碰撞箱高度
    static constexpr float SHADOW_DISTANCE = 300.0F; // 仅计算 x 轴位移
    static constexpr float SHADOW_LIFETIME = 10.0F; // 影子最长存在时间
    static constexpr float DAMAGE_TEXT_LIFETIME = 1.0F; // 伤害数字显示时长
    static constexpr float ATTACK_EFFECT_LIFETIME = 0.15F; // 一次刀刃特效显示时长
    static constexpr float ATTACK_RANGE = 16.0F; // 近战攻击框在正前方延伸的距离
    static constexpr float DEFENSE_RANGE = 8.0F; // 防御判定框在正前方延伸的距离
    static constexpr float PROJECTILE_RADIUS = 4.0F; // 圆形子弹半径
    static constexpr float PROJECTILE_SPEED = 300.0F; // 所有子弹每秒飞行距离
    static constexpr float PROJECTILE_MAX_DISTANCE = 500.0F; // 子弹未命中时的最大飞行距离
    static constexpr float ENEMY_SPACING = 100.0F; // 两个敌军生成位置的水平间隔
    static constexpr float ENEMY_DETECTION_RANGE = 300.0F; // 敌军寻找非同阵营目标的水平范围
    static constexpr int ENEMY_EXPERIENCE_REWARD = 5; // 每击杀一名敌军奖励的经验值
};
