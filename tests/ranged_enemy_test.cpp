#include "player.h"
#include "ranged_enemy.h"

#include <gtest/gtest.h>

TEST(RangedEnemyTest, CopiesPlayerCombatStatsAndFacesLeft) {
    Player player("Player");
    RangedEnemy enemy(player);

    EXPECT_FLOAT_EQ(enemy.getHealth(), player.getHealth());
    EXPECT_FLOAT_EQ(enemy.getMaxHealth(), player.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getAttackDamage(), player.getAttackDamage());
    EXPECT_FLOAT_EQ(enemy.getAttacksPerSecond(), 1.25F);
    EXPECT_FALSE(enemy.isFacingRight());
    EXPECT_EQ(enemy.getFaction(), Faction::Enemy);
}

TEST(RangedEnemyTest, UsesItsConfiguredAttackCooldown) {
    Player player("Player");
    RangedEnemy enemy(player);

    enemy.update(0.8F);

    EXPECT_TRUE(enemy.tryAttack());
    EXPECT_FALSE(enemy.tryAttack());
}
