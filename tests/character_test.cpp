#include "character.h"

#include <gtest/gtest.h>

TEST(CharacterTest, StartsAliveAndUsesDefaultGravityScale) {
    Character character("Test Character");

    EXPECT_TRUE(character.isAlive());
    EXPECT_FLOAT_EQ(character.getGravityScale(), 1.0F);
}

TEST(CharacterTest, RejectsNegativeGravityScale) {
    Character character("Test Character");

    character.setGravityScale(2.0F);
    character.setGravityScale(-1.0F);

    EXPECT_FLOAT_EQ(character.getGravityScale(), 2.0F);
}

TEST(CharacterTest, ProvidesAnOverridableRangedAttackFramework) {
    Character character("Test Character");

    character.rangedAttack();

    SUCCEED();
}
