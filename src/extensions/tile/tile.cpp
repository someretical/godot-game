#include "tile.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Tile::_bind_methods() {
}

Tile::Tile() {
}

Tile::Tile(Root *root, Vector2i tile_index) : m_root(root), m_tile_index(tile_index) {
}

Tile::~Tile() {
}

void Tile::_process(double delta) {
    if (
        m_tile_index.x < 0 || m_tile_index.y < 0 || 
        m_tile_index.x >= m_root->m_curmap.dimensions.x || 
        m_tile_index.y >= m_root->m_curmap.dimensions.y
    ) {
        set_visible(false);
        return;
    }

    auto index = m_root->m_curmap.tile_data[m_tile_index.y][m_tile_index.x];

    if (index == -1) {
        set_visible(false);
        return;
    }

    set_texture(
        m_root->m_tile_preloader->get_resource(m_root->m_tile_preloader->get_resource_list()[index])
    );
    set_visible(true);
}

void Tile::_physics_process(double delta) {
    auto curpos = get_position();
    auto camdelta = m_root->m_camera_vel;

    if (camdelta.x == 0 && camdelta.y == 0) {
        return;
    }

    const int x_mult = (curpos.x - camdelta.x < -TILE_SIZE) - (curpos.x - camdelta.x > (MAP_WIDTH * TILE_SIZE));
    const int y_mult = (curpos.y - camdelta.y < -TILE_SIZE) - (curpos.y - camdelta.y > (MAP_HEIGHT * TILE_SIZE));

    curpos.x += x_mult * TILE_COUNT_X * TILE_SIZE;
    m_tile_index.x += x_mult * TILE_COUNT_X;
    curpos.y += y_mult * TILE_COUNT_Y * TILE_SIZE;
    m_tile_index.y += y_mult * TILE_COUNT_Y;
    
    // if (curpos.x - camdelta.x > ((MAP_WIDTH * TILE_SIZE)))
    // {
    //     curpos.x -= TILE_COUNT_X * TILE_SIZE;
    //     m_tile_index.x -= TILE_COUNT_X;
    // }
    // else if (curpos.x - camdelta.x < -TILE_SIZE)
    // {
    //     curpos.x += TILE_COUNT_X * TILE_SIZE;
    //     m_tile_index.x += TILE_COUNT_X;
    // }
    
    // if (curpos.y - camdelta.y > ((MAP_HEIGHT * TILE_SIZE)))
    // {
    //     curpos.y -= TILE_COUNT_Y * TILE_SIZE;
    //     m_tile_index.y -= TILE_COUNT_Y;
    // }
    // else if (curpos.y - camdelta.y < -TILE_SIZE)
    // {
    //     curpos.y += TILE_COUNT_Y * TILE_SIZE;
    //     m_tile_index.y += TILE_COUNT_Y;
    // }

    set_position(curpos - camdelta);
}
