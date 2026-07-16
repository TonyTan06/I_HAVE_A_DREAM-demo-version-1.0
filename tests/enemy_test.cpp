#include "enemy.h"

#include <gtest/gtest.h>

TEST(EnemyTest, StartsAliveAndHasNoLootByDefault) {
    Enemy enemy("Training Dummy");

    EXPECT_TRUE(enemy.isAlive());
    EXPECT_TRUE(enemy.dropLoot().empty());
}

TEST(EnemyTest, CalculatesExperienceRewardFromEnemyLevel) {
    Enemy levelThreeEnemy("Veteran", 3);

    EXPECT_EQ(levelThreeEnemy.getLevel(), 3);
    EXPECT_EQ(levelThreeEnemy.getExperienceReward(), 15);
}

TEST(EnemyTest, ClampsInvalidLevelToOneForExperienceReward) {
    Enemy invalidLevelEnemy("Invalid", 0);

    EXPECT_EQ(invalidLevelEnemy.getLevel(), 1);
    EXPECT_EQ(invalidLevelEnemy.getExperienceReward(), 5);
}
