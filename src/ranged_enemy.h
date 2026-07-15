#pragma once

#include "enemy.h"

class Player;

class RangedEnemy : public Enemy {
public:
    explicit RangedEnemy(const Player& player);

    bool isFacingRight() const;

private:
    bool isFacingRight_;
};
