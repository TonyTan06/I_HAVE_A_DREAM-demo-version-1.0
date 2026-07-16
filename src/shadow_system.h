#pragma once

#include "player_shadow.h"
#include "raylib.h"

#include <optional>

class Player;

// ShadowSystem 负责影子的记录点、生成距离、实体更新、生命周期和绘制。
// Scene 只通过 getShadow() 取得影子，将它加入攻击或弹道目标列表。
class ShadowSystem {
public:
    explicit ShadowSystem(const Player& player); // 以玩家出生位置建立第一个影子记录点

    void update(const Player& player, float deltaTime); // 更新距离累计、生成、重力和生命周期
    void resetPlayerTracking(const Player& player); // 玩家瞬移或复活后同步位置，避免误算移动距离
    void draw(float platformY, float entityWidth, float entityHeight) const;

    bool hasShadow() const; // 当前是否存在可受击的影子实体
    PlayerShadow* getShadow(); // 返回当前影子；不存在时返回 nullptr
    const PlayerShadow* getShadow() const; // const 场景绘制或只读查询使用

private:
    std::optional<PlayerShadow> shadow_; // 同时最多存在一个影子实体
    float recordedPlayerX_; // 当前记录点的玩家 x 坐标，影子会在这里生成
    float recordedPlayerY_; // 当前记录点的玩家离地高度，允许影子在空中生成
    float previousPlayerX_; // 上一帧玩家 x 坐标，用于累计真实水平移动距离
    float accumulatedHorizontalDistance_; // 本轮尚未生成影子时累计的水平移动距离
    float elapsedLifetime_; // 当前影子已经存在的时间，单位：秒

    static constexpr float SPAWN_DISTANCE = 300.0F; // 累计水平移动达到该距离后生成影子
    static constexpr float LIFETIME = 10.0F; // 影子的最长存在时间
};
