#pragma once

#include "character.h"

class Player;

class PlayerShadow : public Character {
public:
    // 在记录点创建影子，并快照玩家当下的战斗与跳跃属性。
    PlayerShadow(const Player& player, float x, float y);

    int getJumpCount() const; // 返回快照中已使用的跳跃次数
    int getMaxJumpCount() const; // 返回快照中可用的最大跳跃次数

private:
    // 影子不响应玩家输入，因此单独保存生成瞬间的跳跃状态。
    int jumpCount_;
    int maxJumpCount_;
};
