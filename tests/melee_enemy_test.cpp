#include "melee_enemy.h"
#include "player.h"

#include <gtest/gtest.h>

TEST(MeleeEnemyTest, CopiesPlayerCombatStatsAndFacesLeft) {
    Player player("Player");
    MeleeEnemy enemy(player);

    EXPECT_FLOAT_EQ(enemy.getHealth(), player.getHealth());
    EXPECT_FLOAT_EQ(enemy.getMaxHealth(), player.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getAttackDamage(), player.getAttackDamage());
    EXPECT_FLOAT_EQ(enemy.getAttacksPerSecond(), 2.0F);
    EXPECT_FALSE(enemy.isFacingRight());
    EXPECT_EQ(enemy.getFaction(), Faction::Enemy);
}

TEST(MeleeEnemyTest, DoesNotDamageAnEnemyFactionTarget) {
    Player player("Player");
    MeleeEnemy attacker(player);
    MeleeEnemy ally(player);

    attacker.update(0.5F);

    EXPECT_FALSE(attacker.attack(ally));
    EXPECT_FLOAT_EQ(ally.getHealth(), ally.getMaxHealth());
}

TEST(MeleeEnemyTest, RespawnsAtDeathPositionAfterFiveSeconds) {
    Player player("Player");
    MeleeEnemy enemy(player);
    enemy.setPosition(200.0F, 0.0F);
    enemy.takeDamage(enemy.getMaxHealth());

    enemy.update(4.0F);
    EXPECT_FALSE(enemy.isAlive());
    EXPECT_TRUE(enemy.isRespawning());
    EXPECT_FLOAT_EQ(enemy.getRespawnProgress(), 0.8F);

    enemy.update(1.0F);
    EXPECT_TRUE(enemy.isAlive());
    EXPECT_FALSE(enemy.isRespawning());
    EXPECT_FLOAT_EQ(enemy.getHealth(), enemy.getMaxHealth());
    EXPECT_FLOAT_EQ(enemy.getX(), 200.0F);
}
