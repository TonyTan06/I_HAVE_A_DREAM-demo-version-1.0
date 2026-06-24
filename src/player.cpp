#include "player.h"
#include <utility>
#include <iostream>
#include <cmath>

Player::Player(std::string name)
    : Character(std::move(name)),
      jumpCount_(0),
      maxJumpCount_(2),
      experience_(1),
      level_(1),
      gold_(0){
        setGravityScale(1.0F);
      }
// 创建了主角，并且可以二连跳

void Player::moveRight(float deltaTime) {
    Character::moveRight(deltaTime);
} //主角右移

void Player::moveLeft(float deltaTime) {
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
    /*
    maxHealth_ += ;
    health_ = maxHealth_;
    attackDamage_ += ;
    attacksPerSecond_ += ;
    physicalDefense_ += ;
    magicalDefense_ += ;
    */

    std::cout << "LEVEL UP!" << std::endl;
}

void Player::attack() {
    std::cout << "Attack!!!" << std::endl;
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
