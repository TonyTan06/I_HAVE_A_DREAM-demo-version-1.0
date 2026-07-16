#include "combat_system.h"

#include "melee_enemy.h"
#include "player.h"
#include "player_shadow.h"

#include <gtest/gtest.h>

namespace {
CombatSystem makeCombatSystem() {
    return CombatSystem(32.0F, 48.0F, 16.0F, 8.0F);
}
} // namespace

TEST(CombatSystemTest, FindsNearestValidEnemyTarget) {
    Player player("Player");
    player.setPosition(200.0F, 0.0F);
    PlayerShadow shadow(player, 100.0F, 0.0F);
    MeleeEnemy enemy(player);
    enemy.setPosition(0.0F, 0.0F);
    CombatSystem system = makeCombatSystem();

    EXPECT_EQ(system.findNearestEnemyTarget(enemy, player, &shadow), &shadow);
}

TEST(CombatSystemTest, PlayerMeleePrioritizesShadowBeforeEnemy) {
    Player player("Player");
    player.setPosition(100.0F, 0.0F);
    PlayerShadow shadow(player, 132.0F, 0.0F);
    MeleeEnemy enemy(player);
    enemy.setPosition(132.0F, 0.0F);
    CombatSystem system = makeCombatSystem();

    const auto result = system.playerMeleeAttack(player, &shadow, {&enemy}, 360.0F);

    EXPECT_EQ(result.target, &shadow);
    EXPECT_FLOAT_EQ(shadow.getHealth(), shadow.getMaxHealth() - player.getAttackDamage());
    EXPECT_FLOAT_EQ(enemy.getHealth(), enemy.getMaxHealth());
}

TEST(CombatSystemTest, EnemyMeleeCanBeBlockedByPlayerDefense) {
    Player player("Player");
    player.setPosition(140.0F, 0.0F);
    player.setDefending(true);
    MeleeEnemy enemy(player);
    enemy.setPosition(180.0F, 0.0F);
    enemy.update(0.5F);
    CombatSystem system = makeCombatSystem();

    const auto result = system.enemyMeleeAttack(enemy, player, nullptr, 360.0F);

    EXPECT_TRUE(result.blocked);
    EXPECT_FLOAT_EQ(player.getHealth(), player.getMaxHealth());
}

TEST(CombatSystemTest, EnemyMeleeDamagesShadowWhenItIsNearest) {
    Player player("Player");
    player.setPosition(500.0F, 0.0F);
    PlayerShadow shadow(player, 100.0F, 0.0F);
    MeleeEnemy enemy(player);
    enemy.setPosition(116.0F, 0.0F);
    enemy.update(0.5F);
    CombatSystem system = makeCombatSystem();

    const auto result = system.enemyMeleeAttack(enemy, player, &shadow, 360.0F);

    EXPECT_EQ(result.target, &shadow);
    EXPECT_TRUE(result.hit);
    EXPECT_FLOAT_EQ(shadow.getHealth(), shadow.getMaxHealth() - enemy.getAttackDamage());
}

TEST(CombatSystemTest, MeleeEnemyOnlyDetectsTargetsWithinItsOwnRange) {
    Player player("Player");
    MeleeEnemy enemy(player);
    CombatSystem system = makeCombatSystem();
    enemy.setPosition(100.0F, 0.0F);

    player.setPosition(251.0F, 0.0F);
    EXPECT_EQ(system.findNearestEnemyTarget(enemy, player, nullptr), nullptr);

    player.setPosition(250.0F, 0.0F);
    EXPECT_EQ(system.findNearestEnemyTarget(enemy, player, nullptr), &player);
}

TEST(CombatSystemTest, MeleeEnemyTurnsRightBeforeHittingRightSideTarget) {
    Player player("Player");
    MeleeEnemy enemy(player);
    CombatSystem system = makeCombatSystem();
    enemy.setPosition(100.0F, 0.0F);
    player.setPosition(132.0F, 0.0F);
    enemy.update(0.5F);

    const auto result = system.enemyMeleeAttack(enemy, player, nullptr, 360.0F);

    EXPECT_TRUE(enemy.isFacingRight());
    EXPECT_TRUE(result.hit);
    EXPECT_EQ(result.target, &player);
    EXPECT_FLOAT_EQ(player.getHealth(),
                    player.getMaxHealth() - enemy.getAttackDamage());
}
