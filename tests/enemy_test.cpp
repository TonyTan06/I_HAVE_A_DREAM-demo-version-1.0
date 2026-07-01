#include "enemy.h"

#include <gtest/gtest.h>

TEST(EnemyTest, StartsAliveAndHasNoLootByDefault) {
    Enemy enemy("Training Dummy");

    EXPECT_TRUE(enemy.isAlive());
    EXPECT_TRUE(enemy.dropLoot().empty());
}
