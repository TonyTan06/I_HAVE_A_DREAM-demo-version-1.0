#include "enemy.h"

#include <iostream>
#include <utility>

Enemy::Enemy(std::string name)
    : Character(std::move(name))
{
    setGravityScale(1.0F);
}

void Enemy::update(float deltaTime) {
    Character::update(deltaTime);

    if (!isAlive()) return;

    // 敌人独有的 AI、巡逻、追踪等逻辑以后写在这里。
}

void Enemy::attack() {
    if (!isAlive()) return;

    std::cout << name_ << " attacks!" << std::endl;
}
