#include "player.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include <cmath>

Player::Player(std::string name)
    : Character(std::move(name)),
      jumpCount_(0),
      maxJumpCount_(2),
      experience_(0),
      level_(1),
      gold_(0),
      isFacingRight_(true),
      rangedAttackCooldown_(0.0F),
      rangedAttackRequested_(false),
      isDefending_(false),
      defenseCooldown_(0.0F) {
        setGravityScale(1.0F);
        setFaction(Faction::Friendly);
        x_ = 140.0F;
        moveSpeed_ = 240.0F;
        health_ = 10.0F;
        maxHealth_ = 10.0F;
        attackDamage_ = 5.0F;
        jumpInitialVelocity_ = 360.0F;
      }
// 创建了主角，并且可以二连跳

void Player::moveRight(float deltaTime) {
    isFacingRight_ = true;
    Character::moveRight(deltaTime);
} //主角右移

void Player::moveLeft(float deltaTime) {
    isFacingRight_ = false;
    Character::moveLeft(deltaTime);
} //主角左移

void Player::jump() {
    if (jumpCount_ >= maxJumpCount_) return;

    verticalVelocity_ = jumpInitialVelocity_;
    isGrounded_ = false;
    ++jumpCount_;
} //角色跳跃

void Player::update(float deltaTime) {
    Character::update(deltaTime);

    rangedAttackCooldown_ = std::max(0.0F, rangedAttackCooldown_ - deltaTime);
    defenseCooldown_ = std::max(0.0F, defenseCooldown_ - deltaTime);
    if (defenseCooldown_ > 0.0F) {
        isDefending_ = false;
    }

    if (isGrounded_) jumpCount_ = 0;
} //主角的状态更新，主角独有的加在这里面

void Player::addGold(int value) {
    if (value <= 0) return;

    gold_ += value;
}

void Player::addExperience(int value) {
    if (value <= 0) return;

    experience_ += value;

    while (experience_ >= getExperienceThreshold()) {
        levelUp();
    }
}

void Player::levelUp() {
    experience_ -= getExperienceThreshold();
    ++level_;
    maxHealth_ += 5.0F;
    health_ = maxHealth_;
    attackDamage_ += 1.0F;
    attacksPerSecond_ += 0.1F;
    /*
    physicalDefense_ += ;
    magicalDefense_ += ;
    */

    std::cout << "LEVEL UP!" << std::endl;
}

void Player::attack() {
    std::cout << "Attack!!!" << std::endl;
}

void Player::attack(Character& target) {
    target.takeDamage(attackDamage_);
}

void Player::rangedAttack() {
    if (isDefending_) return;
    if (rangedAttackCooldown_ > 0.0F) return;

    rangedAttackCooldown_ = RANGED_ATTACK_INTERVAL;
    rangedAttackRequested_ = true;
}

bool Player::isFacingRight() const {
    return isFacingRight_;
}

bool Player::consumeRangedAttackRequest() {
    const bool wasRequested = rangedAttackRequested_;
    rangedAttackRequested_ = false;
    return wasRequested;
}

void Player::setDefending(bool shouldDefend) {
    isDefending_ = shouldDefend && defenseCooldown_ <= 0.0F;
    if (isDefending_) {
        rangedAttackRequested_ = false;
    }
}

bool Player::isDefending() const {
    return isDefending_;
}

bool Player::blockNextAttack() {
    if (!isDefending_) return false;

    isDefending_ = false;
    defenseCooldown_ = DEFENSE_COOLDOWN;
    return true;
}

int Player::getExperienceThreshold() const{
    return static_cast<int>(10.0F * (level_ + level_ * std::log(level_)));
}

int Player::getJumpCount() const {
    return jumpCount_;
} //获取当前跳跃次数

int Player::getMaxJumpCount() const {
    return maxJumpCount_;
} //获取最大跳跃次数

int Player::getGold() const {
    return gold_;
} //获取当前金币数量

int Player::getExperience() const {
    return experience_;
} //显示当前经验值

int Player::getLevel() const {
    return level_;
} //显示当前等级
