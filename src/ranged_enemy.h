#pragma once

#include "enemy.h"

class Player;

class RangedEnemy : public Enemy {
public:
    explicit RangedEnemy(const Player& player);

    bool isFacingRight() const; // 返回远程兵当前朝向，供绘制和发射方向使用

private:
    bool isFacingRight_; // 远程兵固定为 false，即子弹默认向左飞行
};
