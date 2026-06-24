#pragma once

#include "character.h"
#include <string>

class Player : public Character {
public:
    explicit Player(std::string name); //创建了主角

    void moveRight(float deltaTime); //右移
    void moveLeft(float deltaTime); //左移
    void jump() override; //跳跃
    void update(float deltaTime, float gravity) override; //主角状态更新

    int getJumpCount() const; //跳跃次数（可以实现二连跳）
    int getMaxJumpCount() const; //最大跳跃次数



private:
    int jumpCount_;
    int maxJumpCount_;
    float gravityScale_; //单位是g

};