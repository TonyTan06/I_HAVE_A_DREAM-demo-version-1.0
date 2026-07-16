#include "shadow_system.h"

#include "player.h"

#include <gtest/gtest.h>

TEST(ShadowSystemTest, GeneratesShadowAtRecordedPointAfterThreeHundredPixels) {
    Player player("Player");
    ShadowSystem system(player);
    const float recordedX = player.getX();

    player.moveRight(1.25F); // 240px/s × 1.25s = 300px
    system.update(player, 0.0F);

    ASSERT_TRUE(system.hasShadow());
    ASSERT_NE(system.getShadow(), nullptr);
    EXPECT_FLOAT_EQ(system.getShadow()->getX(), recordedX);
}

TEST(ShadowSystemTest, RemovesShadowAfterTenSecondsAndStartsNewCycle) {
    Player player("Player");
    ShadowSystem system(player);
    player.moveRight(1.25F);
    system.update(player, 0.0F);

    system.update(player, 10.0F);

    EXPECT_FALSE(system.hasShadow());
    EXPECT_EQ(system.getShadow(), nullptr);
}

TEST(ShadowSystemTest, RemovesShadowWhenItsHealthReachesZero) {
    Player player("Player");
    ShadowSystem system(player);
    player.moveRight(1.25F);
    system.update(player, 0.0F);
    ASSERT_NE(system.getShadow(), nullptr);
    system.getShadow()->takeDamage(system.getShadow()->getMaxHealth());

    system.update(player, 0.0F);

    EXPECT_FALSE(system.hasShadow());
}
