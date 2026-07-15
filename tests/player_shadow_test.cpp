#include "player.h"
#include "player_shadow.h"

#include <gtest/gtest.h>

TEST(PlayerShadowTest, CopiesPlayerStatsAndRecordedPosition) {
    Player player("Player");
    player.jump();
    PlayerShadow shadow(player, 25.0F, 40.0F);

    EXPECT_FLOAT_EQ(shadow.getX(), 25.0F);
    EXPECT_FLOAT_EQ(shadow.getY(), 40.0F);
    EXPECT_FLOAT_EQ(shadow.getMoveSpeed(), player.getMoveSpeed());
    EXPECT_FLOAT_EQ(shadow.getPhysicalDefense(), player.getPhysicalDefense());
    EXPECT_FLOAT_EQ(shadow.getMagicalDefense(), player.getMagicalDefense());
    EXPECT_FLOAT_EQ(shadow.getMaxHealth(), player.getMaxHealth());
    EXPECT_FLOAT_EQ(shadow.getHealth(), player.getHealth());
    EXPECT_FLOAT_EQ(shadow.getAttackDamage(), player.getAttackDamage());
    EXPECT_FLOAT_EQ(shadow.getAttacksPerSecond(), player.getAttacksPerSecond());
    EXPECT_FLOAT_EQ(shadow.getShield(), player.getShield());
    EXPECT_FLOAT_EQ(shadow.getJumpInitialVelocity(), player.getJumpInitialVelocity());
    EXPECT_FLOAT_EQ(shadow.getGravityScale(), player.getGravityScale());
    EXPECT_EQ(shadow.getJumpCount(), player.getJumpCount());
    EXPECT_EQ(shadow.getMaxJumpCount(), player.getMaxJumpCount());
    EXPECT_EQ(shadow.getFaction(), Faction::Neutral);
}

TEST(PlayerShadowTest, CanReceiveDamageFromPlayerAttack) {
    Player player("Player");
    PlayerShadow shadow(player, 0.0F, 0.0F);

    player.attack(shadow);

    EXPECT_FLOAT_EQ(shadow.getHealth(), 5.0F);
}

TEST(PlayerShadowTest, IsGroundedOnlyWhenCreatedOnTheGround) {
    Player player("Player");
    PlayerShadow groundedShadow(player, 0.0F, 0.0F);
    PlayerShadow airborneShadow(player, 0.0F, 20.0F);

    EXPECT_TRUE(groundedShadow.isGrounded());
    EXPECT_FALSE(airborneShadow.isGrounded());
}
