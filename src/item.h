#pragma once

#include <string>

enum class ItemRarity {
    Common,     // 普通
    Uncommon,   // 优良
    Rare,       // 稀有
    Epic,       // 史诗
    Legendary,  // 传说
    Mythic      // 神话
};

class Item {
public:
    Item(std::string name, ItemRarity rarity, int basePrice);

    const std::string& getName() const;
    ItemRarity getRarity() const;
    std::string getRarityName() const;
    float getRarityMultiplier() const;
    int getBasePrice() const;
    int getSellPrice() const;

private:
    std::string name_;
    ItemRarity rarity_;
    int basePrice_;
};
