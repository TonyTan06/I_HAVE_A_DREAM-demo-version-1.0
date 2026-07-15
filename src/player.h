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
    void attack(Character& target); // 对目标造成当前攻击伤害
    void rangedAttack() override; // 请求一次远程攻击，受自身攻击间隔限制
    bool isFacingRight() const; // 当前朝向，供眼睛与近战攻击判定使用
    bool consumeRangedAttackRequest(); // 供场景读取并清除本帧的子弹生成请求
    void setDefending(bool shouldDefend); // 按住防御键时尝试进入防御状态
    bool isDefending() const;
    bool blockNextAttack(); // 防御判定命中后抵挡一次攻击并开始冷却

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
    bool isFacingRight_; // 最近一次横向移动方向，初始向右
    float rangedAttackCooldown_; // 下次可发射前的剩余冷却时间
    bool rangedAttackRequested_; // 本帧是否需要由场景生成子弹
    bool isDefending_;
    float defenseCooldown_;

    static constexpr float RANGED_ATTACK_INTERVAL = 0.75F;
    static constexpr float DEFENSE_COOLDOWN = 2.0F;

};
