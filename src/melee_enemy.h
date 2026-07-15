#pragma once

#include "enemy.h"

class Player;

class MeleeEnemy : public Enemy {
public:
    explicit MeleeEnemy(const Player& player);

    bool isFacingRight() const;

private:
    bool isFacingRight_;
};
