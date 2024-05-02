#include "tile.h"
#include "level.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Tile::_bind_methods() {
}

Tile::Tile() {
}

Tile::Tile(Level *level, Vector2 pos, Vector2i tile_index) : m_level(level), m_pos(pos), m_tile_index(tile_index) {
    set_process_priority(static_cast<int>(ProcessingPriority::Tiles));
    set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Tiles));
}

Tile::~Tile() {
}

void Tile::_process(double delta) {
    if (
        m_tile_index.x < 0 || m_tile_index.y < 0 || 
        m_tile_index.x >= m_level->m_curmap.dimensions.x || 
        m_tile_index.y >= m_level->m_curmap.dimensions.y
    ) {
        set_visible(false);
        return;
    }

    auto index = m_level->m_curmap.tile_data[m_tile_index.y][m_tile_index.x];

    if (index == -1) {
        set_visible(false);
        return;
    }

    set_texture(
        m_level->m_tile_preloader->get_resource(m_level->m_tile_preloader->get_resource_list()[index])
    );
    set_visible(true);
}

void Tile::_physics_process(double delta) {
    const auto campos = m_level->m_camera_pos;

    if (abs(m_pos.x - campos.x) > TILE_COUNT_X * HALF_TILE) {
        if (m_pos.x < campos.x) {
            m_pos.x += TILE_COUNT_X * TILE_SIZE;
            m_tile_index.x += TILE_COUNT_X;
        } else {
            m_pos.x -= TILE_COUNT_X * TILE_SIZE;
            m_tile_index.x -= TILE_COUNT_X;
        }
    }

    if (abs(m_pos.y - campos.y) > TILE_COUNT_Y * HALF_TILE) {
        if (m_pos.y < campos.y) {
            m_pos.y += TILE_COUNT_Y * TILE_SIZE;
            m_tile_index.y += TILE_COUNT_Y;
        } else {
            m_pos.y -= TILE_COUNT_Y * TILE_SIZE;
            m_tile_index.y -= TILE_COUNT_Y;
        }
    }

    set_position(m_pos - campos);
}
