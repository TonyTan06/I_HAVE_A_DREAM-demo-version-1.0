#include "player.h"

#include <gtest/gtest.h>

TEST(PlayerTest, StartsAtLevelOneWithNoGold) {
    Player player("Johnny");

    EXPECT_EQ(player.getLevel(), 1);
    EXPECT_EQ(player.getGold(), 0);
    EXPECT_EQ(player.getMaxJumpCount(), 2);
    EXPECT_FLOAT_EQ(player.getHealth(), 10.0F);
    EXPECT_FLOAT_EQ(player.getMaxHealth(), 10.0F);
    EXPECT_FLOAT_EQ(player.getAttackDamage(), 5.0F);
    EXPECT_EQ(player.getFaction(), Faction::Friendly);
    EXPECT_TRUE(player.isFacingRight());
}

TEST(PlayerTest, CanRecoverToFullHealthAfterDeath) {
    Player player("Johnny");

    player.takeDamage(player.getMaxHealth());
    player.heal(player.getMaxHealth());

    EXPECT_TRUE(player.isAlive());
    EXPECT_FLOAT_EQ(player.getHealth(), player.getMaxHealth());
}

TEST(PlayerTest, FacesTheDirectionOfItsLastHorizontalMovement) {
    Player player("Johnny");

    player.moveLeft(0.0F);
    EXPECT_FALSE(player.isFacingRight());

    player.moveRight(0.0F);
    EXPECT_TRUE(player.isFacingRight());
}

TEST(PlayerTest, LimitsRangedAttackToOneShotEveryThreeQuartersOfASecond) {
    Player player("Johnny");

    player.rangedAttack();
    EXPECT_TRUE(player.consumeRangedAttackRequest());

    player.rangedAttack();
    EXPECT_FALSE(player.consumeRangedAttackRequest());

    player.update(0.75F);
    player.rangedAttack();
    EXPECT_TRUE(player.consumeRangedAttackRequest());
}

TEST(PlayerTest, BlocksOneAttackThenHasTwoSecondCooldown) {
    Player player("Johnny");
    player.setDefending(true);

    EXPECT_TRUE(player.isDefending());
    EXPECT_TRUE(player.blockNextAttack());
    EXPECT_FALSE(player.isDefending());

    player.setDefending(true);
    EXPECT_FALSE(player.isDefending());
    player.update(2.0F);
    player.setDefending(true);
    EXPECT_TRUE(player.isDefending());
}

TEST(PlayerTest, DefensePreventsRangedAttackRequest) {
    Player player("Johnny");
    player.setDefending(true);
    player.rangedAttack();

    EXPECT_FALSE(player.consumeRangedAttackRequest());
}

TEST(PlayerTest, AddsOnlyPositiveGold) {
    Player player("Johnny");

    player.addGold(25);
    player.addGold(0);
    player.addGold(-5);

    EXPECT_EQ(player.getGold(), 25);
}

TEST(PlayerTest, CannotJumpMoreThanItsMaximumCount) {
    Player player("Johnny");

    player.jump();
    player.jump();
    player.jump();

    EXPECT_EQ(player.getJumpCount(), player.getMaxJumpCount());
}

TEST(PlayerTest, LevelsUpWhenExperienceReachesThreshold) {
    Player player("Johnny");
    const int threshold = player.getExperienceThreshold();

    player.addExperience(threshold);

    EXPECT_EQ(player.getLevel(), 2);
}
