#pragma once

#include "enemy.h"

class Player;

class MeleeEnemy : public Enemy {
public:
    explicit MeleeEnemy(const Player& player, int level = 1);

    bool isFacingRight() const; // 返回近战兵当前朝向，供绘制和攻击框使用
    void faceToward(const Character& target); // 根据检测到的目标水平位置更新朝向

private:
    bool isFacingRight_; // true 表示面向右侧，false 表示面向左侧
};
