#include "melee_enemy.h"

#include "player.h"

MeleeEnemy::MeleeEnemy(const Player& player)
    : Enemy("MELEE_ENEMY"),
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
    attackCooldown_ = 0.5F;
}

bool MeleeEnemy::isFacingRight() const {
    return isFacingRight_;
}
