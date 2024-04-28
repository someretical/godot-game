#include "tile.h"
#include "level.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Tile::_bind_methods() {
}

Tile::Tile() {
}

Tile::Tile(Level *level, Vector2 pos, Vector2i tile_index) : m_level(level), m_pos(pos), m_tile_index(tile_index) {
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
    // if (abs(curpos.x - campos.x) > SCREEN_TILE_WIDTH * TILE_SIZE) {
    //     if (curpos.x < campos.x) {
    //         curpos.x += SCREEN_TILE_WIDTH * TILE_SIZE;
    //         m_tile_index.x += SCREEN_TILE_WIDTH;
    //     } else {
    //         curpos.x -= SCREEN_TILE_WIDTH * TILE_SIZE;
    //         m_tile_index.x -= SCREEN_TILE_WIDTH;
    //     }
    // }

    // if (abs(curpos.y - campos.y) > SCREEN_TILE_HEIGHT * TILE_SIZE) {
    //     if (curpos.y < campos.y) {
    //         curpos.y += SCREEN_TILE_HEIGHT * TILE_SIZE;
    //         m_tile_index.y += SCREEN_TILE_HEIGHT;
    //     } else {
    //         curpos.y -= SCREEN_TILE_HEIGHT * TILE_SIZE;
    //         m_tile_index.y -= SCREEN_TILE_HEIGHT;
    //     }
    // }


    // const auto newx = curpos.x - camdelta.x;
    // const auto newy = curpos.y - camdelta.y;

    // const int x_mult = (newx < -TILE_SIZE / 2) - (newx > (SCREEN_TILE_WIDTH * TILE_SIZE + TILE_SIZE / 2));
    // const int y_mult = (newy < -TILE_SIZE / 2) - (newy > (SCREEN_TILE_HEIGHT * TILE_SIZE + TILE_SIZE / 2));

    // curpos.x += x_mult * TILE_COUNT_X * TILE_SIZE;
    // m_tile_index.x += x_mult * TILE_COUNT_X;
    // curpos.y += y_mult * TILE_COUNT_Y * TILE_SIZE;
    // m_tile_index.y += y_mult * TILE_COUNT_Y;
    
    // if (curpos.x - camdelta.x > ((SCREEN_TILE_WIDTH * TILE_SIZE)))
    // {
    //     curpos.x -= TILE_COUNT_X * TILE_SIZE;
    //     m_tile_index.x -= TILE_COUNT_X;
    // }
    // else if (curpos.x - camdelta.x < -TILE_SIZE)
    // {
    //     curpos.x += TILE_COUNT_X * TILE_SIZE;
    //     m_tile_index.x += TILE_COUNT_X;
    // }
    
    // if (curpos.y - camdelta.y > ((SCREEN_TILE_HEIGHT * TILE_SIZE)))
    // {
    //     curpos.y -= TILE_COUNT_Y * TILE_SIZE;
    //     m_tile_index.y -= TILE_COUNT_Y;
    // }
    // else if (curpos.y - camdelta.y < -TILE_SIZE)
    // {
    //     curpos.y += TILE_COUNT_Y * TILE_SIZE;
    //     m_tile_index.y += TILE_COUNT_Y;
    // }

    const auto campos = m_level->m_camera_pos;
    const auto x = m_pos.x - campos.x;
    const auto y = m_pos.y - campos.y;

    if (abs(x) > TILE_COUNT_X * HALF_TILE) {
        if (m_pos.x < campos.x) {
            m_pos.x += TILE_COUNT_X * TILE_SIZE;
            m_tile_index.x += TILE_COUNT_X;
        } else {
            m_pos.x -= TILE_COUNT_X * TILE_SIZE;
            m_tile_index.x -= TILE_COUNT_X;
        }
    }

    if (abs(y) > TILE_COUNT_Y * HALF_TILE) {
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
