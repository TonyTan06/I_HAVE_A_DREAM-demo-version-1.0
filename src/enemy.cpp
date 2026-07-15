#include "enemy.h"

#include <iostream>
#include <utility>
#include <random>

Enemy::Enemy(std::string name)
    : Character(std::move(name)),
      detectionRange_(1.0F), //检测范围
      attackRange_(1.0F), //攻击范围
      experienceReward_(0), //击杀经验奖励
      isAggro_(false), //是否进入仇恨状态
      attackCooldown_(1.0F), //攻击冷却时间
      timeSinceLastAttack_(0.0F) //自上次攻击以来的时间
{
    setGravityScale(1.0F);
    setFaction(Faction::Enemy);
}

std::vector<LootItem> Enemy::dropLoot() {
    std::vector<LootItem> droppedItems; // 存储本次实际掉落的物品

    static std::random_device rd; // 创建随机数种子（只初始化一次）
    static std::mt19937 generator(rd()); // Mersenne Twister 随机数引擎（只初始化一次）

    std::uniform_real_distribution<float> chanceDistribution(0.0F, 1.0F); // 生成 [0.0, 1.0] 范围内的随机浮点数

    // 遍历当前敌人的掉落表
    for (const auto& item : lootTable_) {
        float roll = chanceDistribution(generator); // 为当前物品进行一次掉落判定

        if (roll <= item.dropChance) droppedItems.push_back(item); // 如果随机数小于等于掉落概率，则掉落该物品
    }

    // 返回最终掉落结果
    return droppedItems;
}

void Enemy::update(float deltaTime)
{
    Character::update(deltaTime);

    if (!isAlive()) return;

    timeSinceLastAttack_ += deltaTime;

    // 以后这里写：
    // 1. 检测玩家距离
    // 2. 巡逻
    // 3. 追踪玩家
    // 4. 进入攻击范围后 attack()
}

void Enemy::attack()
{
    if (!isAlive()) return;

    if (timeSinceLastAttack_ < attackCooldown_) return; //攻击间隔小于攻击冷却时间则无法攻击

    std::cout << name_ << " attacks!" << std::endl;

    resetAttackCooldown();
}

void Enemy::resetAttackCooldown()
{
    timeSinceLastAttack_ = 0.0F;
}
