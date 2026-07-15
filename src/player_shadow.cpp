#include "player_shadow.h"

#include "player.h"

PlayerShadow::PlayerShadow(const Player& player, float x, float y)
    : Character("PLAYER_SHADOW"),
      jumpCount_(player.getJumpCount()),
      maxJumpCount_(player.getMaxJumpCount()) {
    // 位置来自上一次记录点，而非本次生成时玩家所在的位置。
    setPosition(x, y);
    // 中立单位不参与友敌排除，可被任意阵营的攻击逻辑选中。
    setFaction(Faction::Neutral);
    // 影子的属性在生成瞬间固定，不会随玩家之后的属性变化而改变。
    moveSpeed_ = player.getMoveSpeed();
    physicalDefense_ = player.getPhysicalDefense();
    magicalDefense_ = player.getMagicalDefense();
    health_ = player.getHealth();
    maxHealth_ = player.getMaxHealth();
    attackDamage_ = player.getAttackDamage();
    attacksPerSecond_ = player.getAttacksPerSecond();
    shield_ = player.getShield();
    jumpInitialVelocity_ = player.getJumpInitialVelocity();
    gravityScale_ = player.getGravityScale();
    // 空中生成的影子进入重力更新；地面生成的影子直接保持落地。
    isGrounded_ = y <= 0.0F;
}

int PlayerShadow::getJumpCount() const {
    return jumpCount_;
}

int PlayerShadow::getMaxJumpCount() const {
    return maxJumpCount_;
}
