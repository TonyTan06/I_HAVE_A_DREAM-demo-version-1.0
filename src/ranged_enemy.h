#pragma once

#include "enemy.h"

class Player;

class RangedEnemy : public Enemy {
public:
    explicit RangedEnemy(const Player& player, int level = 1);

    bool isFacingRight() const; // 返回远程兵当前朝向，供绘制和发射方向使用
    void faceToward(const Character& target); // 根据当前目标的水平位置更新朝向

private:
    bool isFacingRight_; // true 表示面向右侧，false 表示面向左侧
};
