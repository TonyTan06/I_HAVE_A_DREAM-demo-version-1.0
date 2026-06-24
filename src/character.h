#pragma once

#include <string>

class Character {
public:
    static constexpr float BASE_GRAVITY = 980.0F; //基础重力加速度，单位大约是像素/秒²，1.0F 代表 1g

    explicit Character(std::string name);//创建人物要填写名字

    void moveRight(float deltaTime); //右移
    void moveLeft(float deltaTime); //左移
    //deltaTime 是上一帧到当前帧经过了多少秒。
    virtual void jump(); //跳跃
    void takeDamage(float damage); //受击伤害
    void heal(float value); //回血量
    virtual void attack(); //通用攻击代码
    virtual void update(float deltaTime); //每一帧更新角色函数
    
    bool isAlive() const; //检测存活
    float getGravityScale() const; //获取角色自身重力倍率
    void setGravityScale(float gravityScale); //设置角色自身重力倍率，1.0F 就是 1g

    virtual ~Character() = default;

protected:
    std::string name_; //角色名字

    float x_; //角色x轴位置信息
    float y_; //角色y轴位置信息
    float moveSpeed_; //角色移动速度
    float physicalDefense_; //角色物理防御
    float magicalDefense_; //角色魔法防御
    float health_; //角色当前血量
    float maxHealth_; //角色最大血量
    float attackDamage_; //角色攻击伤害
    float attacksPerSecond_; //角色每秒攻击速度
    float shield_; //角色护盾值

    //不同角色可以设置不同的重力倍率，影响跳跃高度和下落速度
    float jumpInitialVelocity_; //角色跳跃初速度
    float verticalVelocity_; //角色当前的垂直速度
    float gravityScale_; //角色受到的重力倍率，1.0F 是 1g，0.5F 是半重力，2.0F 是双倍重力

    bool isGrounded_; //检测角色是否下落到实体上

    

private:


};
