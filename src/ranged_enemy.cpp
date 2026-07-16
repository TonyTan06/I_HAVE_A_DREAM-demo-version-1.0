#include "ranged_enemy.h"

#include "player.h"

RangedEnemy::RangedEnemy(const Player& player, int level)
    : Enemy("RANGED_ENEMY", level),
      isFacingRight_(false) {
    moveSpeed_ = player.getMoveSpeed();
    physicalDefense_ = player.getPhysicalDefense();
    magicalDefense_ = player.getMagicalDefense();
    health_ = player.getHealth();
    maxHealth_ = player.getMaxHealth();
    attackDamage_ = player.getAttackDamage();
    attacksPerSecond_ = 1.25F;
    shield_ = player.getShield();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
    detectionRange_ = 300.0F;
    attackCooldown_ = 0.8F;
}

bool RangedEnemy::isFacingRight() const {
    return isFacingRight_;
}

void RangedEnemy::faceToward(const Character& target) {
    // 目标与敌军完全同一水平位置时保留原朝向，避免每帧无意义切换。
    if (target.getX() > getX()) {
        isFacingRight_ = true;
    } else if (target.getX() < getX()) {
        isFacingRight_ = false;
    }
}
