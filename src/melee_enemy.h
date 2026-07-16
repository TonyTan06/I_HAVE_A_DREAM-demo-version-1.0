#pragma once

#include "enemy.h"

class Player;

class MeleeEnemy : public Enemy {
public:
    explicit MeleeEnemy(const Player& player);

    bool isFacingRight() const; // 返回近战兵当前朝向，供绘制和攻击框使用

private:
    bool isFacingRight_; // 近战兵固定为 false，即面向左侧玩家出生区域
};
