#pragma once

#include "character.h"

#include <string>
#include <vector>

struct LootItem {
    std::string itemName; //掉落物品名称
    int minAmout; //最少掉落物品数量
    int maxAmout; //最多掉落物品数量
    float dropChance; //掉落概率，0.0F - 1.0
};

class Enemy : public Character {

public:
    explicit Enemy(std::string name, int level = 1);

    void attack() override;
    bool tryAttack(); // 尝试消耗一次攻击冷却，不直接指定攻击表现
    bool attack(Character& target); // 对非同阵营目标造成伤害
    void update(float deltaTime) override; //敌人状态更新
    bool isRespawning() const; // 当前是否正处于死亡后的复活倒计时
    float getRespawnProgress() const; // 复活倒计时的完成比例，范围 0 到 1
    int getLevel() const; // 返回当前敌军等级，最低为 1 级
    int getExperienceReward() const; // 返回击杀该敌军可获得的经验值
    float getDetectionRange() const; // 返回当前兵种用于选择目标的水平检测距离
    bool isFacingRight() const; // 返回敌军当前水平朝向，默认朝左
    void faceToward(const Character& target); // 根据目标水平位置更新朝向

    std::vector<LootItem> dropLoot(); //掉落物品表

protected:
    float detectionRange_;     // 发现玩家距离
    float attackRange_;        // 攻击距离
    int level_;                // 敌军等级，用于计算击杀经验等奖励数值
    int experienceReward_;     // 击杀获得经验，当前公式为 5 × 敌军等级
    bool isAggro_;             // 是否进入仇恨状态
    bool isFacingRight_;       // true 表示朝右，false 表示朝左

    float attackCooldown_; // 攻击冷却时间
    float timeSinceLastAttack_; // 自上次攻击以来的时间
    bool isRespawning_; // 死亡后第一次 update 起设为 true
    float respawnElapsedTime_; // 本次复活倒计时已过去的时间，单位：秒
    static constexpr float RESPAWN_DURATION = 5.0F; // 敌军从死亡到满血复活所需时间
    void resetAttackCooldown();

    std::vector<LootItem> lootTable_; //掉落物品表

};
