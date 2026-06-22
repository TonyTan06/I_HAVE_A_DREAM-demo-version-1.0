#pragma once

#include <string>

class Player {
public:
    Player(std::string name, float x = 0.0F, float y = 0.0F);

    void move(float directionX, float directionY, float deltaTime);
    void takeDamage(int damage);
    void heal(int amount);

    const std::string& getName() const;
    float getX() const;
    float getY() const;
    float getSpeed() const;
    int getHealth() const;
    int getMaxHealth() const;
    bool isAlive() const;

    void setPosition(float x, float y);
    void setSpeed(float speed);

private:
    std::string name_;
    float x_;
    float y_;
    float speed_;
    int health_;
    int maxHealth_;
};
