#include "player.h"

#include <gtest/gtest.h>

TEST(PlayerTest, StartsAtLevelOneWithNoGold) {
    Player player("Johnny");

    EXPECT_EQ(player.getLevel(), 1);
    EXPECT_EQ(player.getGold(), 0);
    EXPECT_EQ(player.getMaxJumpCount(), 2);
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
