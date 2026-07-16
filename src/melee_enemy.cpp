#include "melee_enemy.h"

#include "player.h"

MeleeEnemy::MeleeEnemy(const Player& player, int level)
    : Enemy("MELEE_ENEMY", level),
      isFacingRight_(false) {
    moveSpeed_ = player.getMoveSpeed();
    physicalDefense_ = player.getPhysicalDefense();
    magicalDefense_ = player.getMagicalDefense();
    health_ = player.getHealth();
    maxHealth_ = player.getMaxHealth();
    attackDamage_ = player.getAttackDamage();
    attacksPerSecond_ = 2.0F;
    shield_ = player.getShield();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
    detectionRange_ = 150.0F;
    attackCooldown_ = 0.5F;
}

bool MeleeEnemy::isFacingRight() const {
    return isFacingRight_;
}

void MeleeEnemy::faceToward(const Character& target) {
    // 目标与近战兵同一水平位置时保留原朝向，避免朝向来回抖动。
    if (target.getX() > getX()) {
        isFacingRight_ = true;
    } else if (target.getX() < getX()) {
        isFacingRight_ = false;
    }
}
