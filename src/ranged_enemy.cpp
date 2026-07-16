#include "ranged_enemy.h"

#include "player.h"

RangedEnemy::RangedEnemy(const Player& player)
    : Enemy("RANGED_ENEMY"),
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
    attackCooldown_ = 0.8F;
}

bool RangedEnemy::isFacingRight() const {
    return isFacingRight_;
}
