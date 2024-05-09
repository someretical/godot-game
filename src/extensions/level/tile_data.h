#ifndef TILE_DATA_H
#define TILE_DATA_H

#include <array>
#include <string>

namespace godot {

namespace tiles {

constexpr std::string_view BLUE_TILES[] = {
    "Blue-1", "Blue-2", "Blue-3", "Blue-4", "Blue-5", "Blue-6", "Blue-7", "Blue-8", "Blue-9"
};

constexpr std::string_view WOOD_TILES[] = {
    "Wood-0", "Wood-1", "Wood-2", "Wood-3", "Wood-4", "Wood-5"
};

constexpr std::string_view BLOCK_GOLD[] = {
    "Block-Gold"
};

constexpr std::string_view BLOCK_WOOD[] = {
    "Block-Wood"
};

constexpr std::string_view COIN1[] = {
    "Coin1", "Coin2", "Coin3", "Coin4"
};

constexpr std::string_view QUESTION1[] = {
    "Question1", "Question2", "Question3", "Question4"
};

constexpr std::array TILE_VARIANTS = {
    BLUE_TILES,
    WOOD_TILES,
    BLOCK_GOLD,
    BLOCK_WOOD,
    COIN1,
    QUESTION1
};

constexpr std::array VARIANT_SIZES = {
    sizeof(BLUE_TILES) / sizeof(BLUE_TILES[0]),
    sizeof(WOOD_TILES) / sizeof(WOOD_TILES[0]),
    sizeof(BLOCK_GOLD) / sizeof(BLOCK_GOLD[0]),
    sizeof(BLOCK_WOOD) / sizeof(BLOCK_WOOD[0]),
    sizeof(COIN1) / sizeof(COIN1[0]),
    sizeof(QUESTION1) / sizeof(QUESTION1[0])
};

}

class TileData {
public:
    static auto get_tile_variants() {
        return tiles::TILE_VARIANTS;
    };

    static auto get_variant_sizes() {
        return tiles::VARIANT_SIZES;
    };
};

}

#endif