#include "combat_system.h"

#include "melee_enemy.h"
#include "player.h"

#include <gtest/gtest.h>

namespace {
CombatSystem makeCombatSystem() {
    return CombatSystem(32.0F, 48.0F, 16.0F, 8.0F);
}
} // namespace

TEST(CombatSystemTest, FindsNearestValidEnemyTarget) {
    Player player("Player");
    player.setPosition(100.0F, 0.0F);
    MeleeEnemy enemy(player);
    enemy.setPosition(0.0F, 0.0F);
    CombatSystem system = makeCombatSystem();

    EXPECT_EQ(system.findNearestEnemyTarget(enemy, player), &player);
}

TEST(CombatSystemTest, PlayerMeleeTargetsEnemy) {
    Player player("Player");
    player.setPosition(100.0F, 0.0F);
    MeleeEnemy enemy(player);
    enemy.setPosition(132.0F, 0.0F);
    CombatSystem system = makeCombatSystem();

    const auto result = system.playerMeleeAttack(player, {&enemy}, 360.0F);

    EXPECT_EQ(result.target, &enemy);
    EXPECT_FLOAT_EQ(enemy.getHealth(), enemy.getMaxHealth() - player.getAttackDamage());
}

TEST(CombatSystemTest, EnemyMeleeCanBeBlockedByPlayerDefense) {
    Player player("Player");
    player.setPosition(140.0F, 0.0F);
    player.setDefending(true);
    MeleeEnemy enemy(player);
    enemy.setPosition(180.0F, 0.0F);
    enemy.update(0.5F);
    CombatSystem system = makeCombatSystem();

    const auto result = system.enemyMeleeAttack(enemy, player, 360.0F);

    EXPECT_TRUE(result.blocked);
    EXPECT_FLOAT_EQ(player.getHealth(), player.getMaxHealth());
}

TEST(CombatSystemTest, MeleeEnemyOnlyDetectsTargetsWithinItsOwnRange) {
    Player player("Player");
    MeleeEnemy enemy(player);
    CombatSystem system = makeCombatSystem();
    enemy.setPosition(100.0F, 0.0F);

    player.setPosition(251.0F, 0.0F);
    EXPECT_EQ(system.findNearestEnemyTarget(enemy, player), nullptr);

    player.setPosition(250.0F, 0.0F);
    EXPECT_EQ(system.findNearestEnemyTarget(enemy, player), &player);
}

TEST(CombatSystemTest, MeleeEnemyTurnsRightBeforeHittingRightSideTarget) {
    Player player("Player");
    MeleeEnemy enemy(player);
    CombatSystem system = makeCombatSystem();
    enemy.setPosition(100.0F, 0.0F);
    player.setPosition(132.0F, 0.0F);
    enemy.update(0.5F);

    const auto result = system.enemyMeleeAttack(enemy, player, 360.0F);

    EXPECT_TRUE(enemy.isFacingRight());
    EXPECT_TRUE(result.hit);
    EXPECT_EQ(result.target, &player);
    EXPECT_FLOAT_EQ(player.getHealth(),
                    player.getMaxHealth() - enemy.getAttackDamage());
}
