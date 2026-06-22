#include "player.h"

#include <algorithm>
#include <cmath>
#include <utility>

Player::Player(std::string name, float x, float y)
    : name_(std::move(name)),
      x_(x),
      y_(y),
      speed_(300.0F),
      health_(100),
      maxHealth_(100) {
}

void Player::move(float directionX, float directionY, float deltaTime) {
    const float length =
        std::sqrt(directionX * directionX + directionY * directionY);

    if (length > 0.0F) {
        directionX /= length;
        directionY /= length;
    }

    x_ += directionX * speed_ * deltaTime;
    y_ += directionY * speed_ * deltaTime;
}

void Player::takeDamage(int damage) {
    if (damage <= 0) {
        return;
    }

    health_ = std::max(0, health_ - damage);
}

void Player::heal(int amount) {
    if (amount <= 0 || !isAlive()) {
        return;
    }

    health_ = std::min(maxHealth_, health_ + amount);
}

const std::string& Player::getName() const {
    return name_;
}

float Player::getX() const {
    return x_;
}

float Player::getY() const {
    return y_;
}

float Player::getSpeed() const {
    return speed_;
}

int Player::getHealth() const {
    return health_;
}

int Player::getMaxHealth() const {
    return maxHealth_;
}

bool Player::isAlive() const {
    return health_ > 0;
}

void Player::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void Player::setSpeed(float speed) {
    speed_ = std::max(0.0F, speed);
}
