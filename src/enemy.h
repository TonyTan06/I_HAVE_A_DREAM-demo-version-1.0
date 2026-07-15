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
    explicit Enemy(std::string name);

    void attack() override;
    bool tryAttack(); // 尝试消耗一次攻击冷却，不直接指定攻击表现
    bool attack(Character& target); // 对非同阵营目标造成伤害
    void update(float deltaTime) override; //敌人状态更新
    bool isRespawning() const;
    float getRespawnProgress() const;

    std::vector<LootItem> dropLoot(); //掉落物品表

protected:
    float detectionRange_;     // 发现玩家距离
    float attackRange_;        // 攻击距离
    int experienceReward_;     // 击杀获得经验
    bool isAggro_;             // 是否进入仇恨状态

    float attackCooldown_; // 攻击冷却时间
    float timeSinceLastAttack_; // 自上次攻击以来的时间
    bool isRespawning_;
    float respawnElapsedTime_;
    static constexpr float RESPAWN_DURATION = 5.0F;
    void resetAttackCooldown();

    std::vector<LootItem> lootTable_; //掉落物品表

};
