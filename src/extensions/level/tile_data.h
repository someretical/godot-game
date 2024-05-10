#ifndef TILE_DATA_H
#define TILE_DATA_H

#include <array>
#include <string>

namespace godot {

namespace tiles {

constexpr std::string_view BLUE_TILES[] = {
    "Blue-1", "Blue-2", "Blue-3", "Blue-4", "Blue-5", "Blue-6", "Blue-7", "Blue-8", "Blue-9"
};

constexpr std::string_view ORANGE_TILES[] = {
    "Orange-1", "Orange-2", "Orange-3", "Orange-4", "Orange-5", "Orange-6", "Orange-7", "Orange-8", "Orange-9"
};

constexpr std::string_view GRASS_TILES[] = {
    "Grass-1", "Grass-2", "Grass-3", "Grass-4", "Grass-5", "Grass-6", "Grass-7", "Grass-8", "Grass-9",
    "Grass-10", "Grass-11", "Grass-12", "Grass-13", "Grass-14", "Grass-15", "Grass-16", "Grass-17", "Grass-18"
};

constexpr std::string_view WOOD_TILES[] = {
    "Wood-0", "Wood-1", "Wood-2", "Wood-3", "Wood-4", "Wood-5"
};

constexpr std::string_view PIPE_TILES[] = {
    "Pipe-1", "Pipe-2", "Pipe-3", "Pipe-4"
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

constexpr std::string_view START_POS[] = {
    "StartPos"
};

constexpr std::array TILE_VARIANTS = {
    BLUE_TILES,
    WOOD_TILES,
    BLOCK_GOLD,
    BLOCK_WOOD,
    COIN1,
    QUESTION1,
    START_POS
};

constexpr std::array VARIANT_SIZES = {
    sizeof(BLUE_TILES) / sizeof(BLUE_TILES[0]),
    sizeof(WOOD_TILES) / sizeof(WOOD_TILES[0]),
    sizeof(BLOCK_GOLD) / sizeof(BLOCK_GOLD[0]),
    sizeof(BLOCK_WOOD) / sizeof(BLOCK_WOOD[0]),
    sizeof(COIN1) / sizeof(COIN1[0]),
    sizeof(QUESTION1) / sizeof(QUESTION1[0]),
    sizeof(START_POS) / sizeof(START_POS[0])
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