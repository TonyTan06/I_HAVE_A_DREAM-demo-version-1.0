#pragma once

#include "character.h"
#include <string>

class Player : public Character {
public:
    explicit Player(std::string name); //创建了主角

    void moveRight(float deltaTime); //右移
    void moveLeft(float deltaTime); //左移
    void jump() override; //跳跃
    void update(float deltaTime) override; //主角状态更新
    void addGold(int value); //获得金币
    void addExperience(int value); //获得经验
    void levelUp(); //主角升级
    void attack() override;

    int getJumpCount() const; //跳跃次数（可以实现二连跳）
    int getMaxJumpCount() const; //最大跳跃次数
    int getGold() const; 
    int getExperience() const;
    int getLevel() const;
    int getExperienceThreshold() const; //升级需要的经验

private:
    int jumpCount_;
    int maxJumpCount_;

    int experience_; //主角经验值
    int level_; //主角等级
    int gold_; //主角金币

};
