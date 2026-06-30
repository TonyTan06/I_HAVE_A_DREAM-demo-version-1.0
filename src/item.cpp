#include "item.h"

#include <algorithm>
#include <utility>

Item::Item(std::string name, ItemRarity rarity, int basePrice)
    : name_(std::move(name)),
      rarity_(rarity),
      basePrice_(std::max(0, basePrice))
{
}

const std::string& Item::getName() const {
    return name_;
}

ItemRarity Item::getRarity() const {
    return rarity_;
}

std::string Item::getRarityName() const {
    switch (rarity_) {
        case ItemRarity::Common:
            return "Common";
        case ItemRarity::Uncommon:
            return "Uncommon";
        case ItemRarity::Rare:
            return "Rare";
        case ItemRarity::Epic:
            return "Epic";
        case ItemRarity::Legendary:
            return "Legendary";
        case ItemRarity::Mythic:
            return "Mythic";
    }

    return "Unknown";
}

float Item::getRarityMultiplier() const {
    switch (rarity_) {
        case ItemRarity::Common:
            return 1.0F;
        case ItemRarity::Uncommon:
            return 1.5F;
        case ItemRarity::Rare:
            return 2.0F;
        case ItemRarity::Epic:
            return 3.0F;
        case ItemRarity::Legendary:
            return 5.0F;
        case ItemRarity::Mythic:
            return 10.0F;
    }

    return 1.0F;
}

int Item::getBasePrice() const {
    return basePrice_;
}

int Item::getSellPrice() const {
    return static_cast<int>(basePrice_ * getRarityMultiplier());
}
