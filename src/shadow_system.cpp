#include "shadow_system.h"

#include "player.h"

#include <algorithm>
#include <cmath>

ShadowSystem::ShadowSystem(const Player& player)
    : shadow_(std::nullopt),
      recordedPlayerX_(player.getX()),
      recordedPlayerY_(player.getY()),
      previousPlayerX_(player.getX()),
      accumulatedHorizontalDistance_(0.0F),
      elapsedLifetime_(0.0F) {
}

void ShadowSystem::update(const Player& player, float deltaTime) {
    const float frameHorizontalDistance = std::abs(player.getX() - previousPlayerX_);
    previousPlayerX_ = player.getX();

    if (shadow_.has_value()) {
        // 影子是实体：空中生成时按 Character 的重力规则下落并检测地面。
        shadow_->update(deltaTime);
        if (shadow_->getY() <= 0.0F) {
            shadow_->land();
        }

        elapsedLifetime_ += std::max(0.0F, deltaTime);
        if (elapsedLifetime_ >= LIFETIME || !shadow_->isAlive()) {
            // 影子消失的这一刻记录玩家位置，下一轮从该位置重新累计 300px。
            shadow_.reset();
            recordedPlayerX_ = player.getX();
            recordedPlayerY_ = player.getY();
            accumulatedHorizontalDistance_ = 0.0F;
            elapsedLifetime_ = 0.0F;
        }
        return;
    }

    // 使用实际位移累计，因此折返、跳跃横移和闪避都会计入生成距离。
    accumulatedHorizontalDistance_ += frameHorizontalDistance;
    if (accumulatedHorizontalDistance_ >= SPAWN_DISTANCE) {
        shadow_.emplace(player, recordedPlayerX_, recordedPlayerY_);
        elapsedLifetime_ = 0.0F;
    }
}

void ShadowSystem::resetPlayerTracking(const Player& player) {
    previousPlayerX_ = player.getX();
    accumulatedHorizontalDistance_ = 0.0F;
}

void ShadowSystem::draw(float platformY, float entityWidth, float entityHeight) const {
    if (!shadow_.has_value()) {
        // 记录点模型只用于提示下一次生成位置，不参与碰撞或受击。
        DrawRectangle(static_cast<int>(recordedPlayerX_),
                      static_cast<int>(platformY - entityHeight - recordedPlayerY_),
                      static_cast<int>(entityWidth), static_cast<int>(entityHeight),
                      Color{120, 170, 255, 80});
        return;
    }

    const Rectangle hitbox{
        shadow_->getX(), platformY - entityHeight - shadow_->getY(),
        entityWidth, entityHeight};
    const float healthRatio = shadow_->getHealth() / shadow_->getMaxHealth();
    const float timeRatio = 1.0F - elapsedLifetime_ / LIFETIME;
    const int barX = static_cast<int>(hitbox.x);
    const int healthBarY = static_cast<int>(hitbox.y - 18.0F);
    const int timeBarY = healthBarY - 7;
    const int barWidth = static_cast<int>(entityWidth);

    DrawRectangle(static_cast<int>(hitbox.x), static_cast<int>(hitbox.y),
                  static_cast<int>(entityWidth), static_cast<int>(entityHeight),
                  Color{72, 183, 255, 100});
    DrawRectangleRec(hitbox, Color{255, 100, 100, 60});
    DrawRectangleLinesEx(hitbox, 2.0F, RED);
    DrawRectangle(barX, healthBarY, barWidth, 5, DARKGRAY);
    DrawRectangle(barX, healthBarY,
                  static_cast<int>(barWidth * std::clamp(healthRatio, 0.0F, 1.0F)),
                  5, RED);
    DrawRectangle(barX, timeBarY, barWidth, 4, DARKGRAY);
    DrawRectangle(barX, timeBarY,
                  static_cast<int>(barWidth * std::clamp(timeRatio, 0.0F, 1.0F)),
                  4, GREEN);
}

bool ShadowSystem::hasShadow() const {
    return shadow_.has_value();
}

PlayerShadow* ShadowSystem::getShadow() {
    return shadow_.has_value() ? &*shadow_ : nullptr;
}

const PlayerShadow* ShadowSystem::getShadow() const {
    return shadow_.has_value() ? &*shadow_ : nullptr;
}
