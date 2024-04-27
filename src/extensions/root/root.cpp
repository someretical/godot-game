#include "root.h"
#include "../tile/tile.h"
#include "../player/player.h"

#include <cmath>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Root::_bind_methods() {
}

Root::Root() {
    m_rng = memnew(RandomNumberGenerator);
    m_rng->randomize();

    m_curmap.dimensions = Vector2{25, 25};
    m_curmap.tile_data = memnew_arr(int *, m_curmap.dimensions.y);
    for (int i = 0; i < m_curmap.dimensions.y; i++) {
        m_curmap.tile_data[i] = memnew_arr(int, m_curmap.dimensions.x);
    }

    for (int i = 1; i < m_curmap.dimensions.y - 1; i++) {
        for (int j = 1; j < m_curmap.dimensions.x - 1; j++) {
            m_curmap.tile_data[i][j] = m_rng->randi_range(0, 10) == 0 ? 2 : -1;
        }
    }

    for (int i = 0; i < m_curmap.dimensions.y; i++) {
        m_curmap.tile_data[i][0] = 0;
        m_curmap.tile_data[i][m_curmap.dimensions.x - 1] = 0;
    }

    for (int i = 0; i < m_curmap.dimensions.x; i++) {
        m_curmap.tile_data[0][i] = 0;
        m_curmap.tile_data[m_curmap.dimensions.y - 1][i] = 0;
    }
}

Root::~Root() {
    delete m_rng;
}

void Root::_ready() {
    m_player_preloader = get_node<ResourcePreloader>("PlayerPreloader");
    m_tile_preloader = get_node<ResourcePreloader>("TilePreloader");
    m_camera = get_node<Camera2D>("Camera");
    m_map_node = get_node<Node>("Map");
    m_tiles_node = get_node<Node>("Map/Tiles");
    m_collectables_node = get_node<Node>("Map/Collectables");
    m_mobs_node = get_node<Node>("Map/Mobs");

    // Setup map tiles
    // Do we need to free the tiles ourselves?
    for (int i = 0; i < TILE_COUNT_Y; i++) {
        for (int j = 0; j < TILE_COUNT_X; j++) {
            // convert to starting from bottom left
            int new_i = m_curmap.dimensions.y - TILE_COUNT_Y + i + 1;
            int new_j = j;

            auto tile = memnew(Tile(this, Vector2i(new_j, new_i)));
            tile->set_position(Vector2(j * TILE_SIZE, i * TILE_SIZE));
            tile->set_centered(false);
            tile->set_process_priority(priorities::ProcessingPriority::Tiles);
            tile->set_physics_process_priority(priorities::PhysicsProcessingPriority::Tiles);
            m_tiles_node->add_child(tile);
        }
    }

    // Setup player
    auto player = memnew(Player(this, Vector2(0, 0)));
    player->set_position(Vector2(0, 0));
    player->set_centered(false);
    player->set_process_priority(priorities::ProcessingPriority::Player);
    player->set_physics_process_priority(priorities::PhysicsProcessingPriority::Player);
    m_mobs_node->add_child(player);

    // Setup camera
    int iy = static_cast<int>(roundl(m_camera->get_viewport_rect().get_size().y) / roundl(m_camera->get_zoom().y));
    int ix = static_cast<int>(roundl(m_camera->get_viewport_rect().get_size().x) / roundl(m_camera->get_zoom().x));
    m_camera_bounds = Rect2i{0, 0, m_curmap.dimensions.x * TILE_SIZE - ix, m_curmap.dimensions.y * TILE_SIZE - iy};

    m_camera_pos = Vector2{0, m_curmap.dimensions.y * TILE_SIZE - (m_camera->get_viewport_rect().get_size().y / m_camera->get_zoom().y)};
}

void Root::_physics_process(double delta) {
    const int maxvel = 2;
    m_camera_vel = Vector2(
        (Input::get_singleton()->is_action_pressed("ui_right") - Input::get_singleton()->is_action_pressed("ui_left")) * maxvel,
        (Input::get_singleton()->is_action_pressed("ui_down") - Input::get_singleton()->is_action_pressed("ui_up")) * maxvel
    );

    m_camera_pos += m_camera_vel;

    if (m_camera_pos.x < 0) {
        m_camera_pos.x = 0;
        m_camera_vel.x = 0;
    } else if (m_camera_pos.x > m_camera_bounds.get_end().x) {
        m_camera_pos.x = m_camera_bounds.size.x;
        m_camera_vel.x = 0;
    }

    if (m_camera_pos.y < 0) {
        m_camera_pos.y = 0;
        m_camera_vel.y = 0;
    } else if (m_camera_pos.y > m_camera_bounds.get_end().y) {
        m_camera_pos.y = m_camera_bounds.size.y;
        m_camera_vel.y = 0;
    }
}
