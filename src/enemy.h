#pragma once

#include "character.h"

#include <string>

class Enemy : public Character {
public:
    explicit Enemy(std::string name);

    void update(float deltaTime) override;
    void attack() override;
};
