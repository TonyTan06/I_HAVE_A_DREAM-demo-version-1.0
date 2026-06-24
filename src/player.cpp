#include "player.h"
#include <utility>

Player::Player(std::string name)
    : Character(std::move(name)),
      jumpCount_(0),
      maxJumpCount_(2),
      gravityScale_(1.0F){
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

void Player::update(float deltaTime, float gravity) {
    Character::update(deltaTime, gravity);

    if (isGrounded_) jumpCount_ = 0;
} //主角的状态更新，主角独有的加在这里面

int Player::getJumpCount() const {
    return jumpCount_;
}

int Player::getMaxJumpCount() const {
    return maxJumpCount_;
}