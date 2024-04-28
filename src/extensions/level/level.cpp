#include "level.h"
#include "tile.h"
#include "player.h"

#include <cmath>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void Level::_bind_methods() {
}

Level::Level() {
    m_rng = memnew(RandomNumberGenerator);
    m_rng->randomize();

    const auto loader = ResourceLoader::get_singleton();

    m_player_preloader = memnew(ResourcePreloader);
    m_player_preloader->add_resource("beacon", loader->load("res://src/assets/tiles/beacon.png"));
    m_player_preloader->add_resource("bedrock", loader->load("res://src/assets/tiles/bedrock.png"));
    m_player_preloader->add_resource("bee_nest_front", loader->load("res://src/assets/tiles/bee_nest_front.png"));
    m_player_preloader->add_resource("blackstone", loader->load("res://src/assets/tiles/blackstone.png"));
    m_player_preloader->add_resource("cobblestone", loader->load("res://src/assets/tiles/cobblestone.png"));
    m_player_preloader->add_resource("cobweb", loader->load("res://src/assets/tiles/cobweb.png"));
    m_player_preloader->add_resource("dirt", loader->load("res://src/assets/tiles/dirt.png"));
    m_player_preloader->add_resource("grass_block_side", loader->load("res://src/assets/tiles/grass_block_side.png"));
    m_player_preloader->add_resource("red_sand", loader->load("res://src/assets/tiles/red_sand.png"));
    m_player_preloader->add_resource("sand", loader->load("res://src/assets/tiles/sand.png"));
    m_player_preloader->add_resource("smooth_stone", loader->load("res://src/assets/tiles/smooth_stone.png"));
    add_child(m_player_preloader);

    m_tile_preloader = memnew(ResourcePreloader);
    add_child(m_tile_preloader);

    m_camera = memnew(Camera2D());
    m_camera->set_zoom(Vector2(2, 2));
    m_camera->set_position(Vector2(0, 0));
    add_child(m_camera);

    m_map_node = memnew(Node);
    add_child(m_map_node);

    m_tiles_node = memnew(Node);
    m_map_node->add_child(m_tiles_node);

    m_collectables_node = memnew(Node);
    m_map_node->add_child(m_collectables_node);

    m_mobs_node = memnew(Node);
    m_map_node->add_child(m_mobs_node);

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

Level::~Level() {
}

void Level::_ready() {
    // // Setup map tiles
    // for (int i = 0; i < TILE_COUNT_Y; i++) {
    //     for (int j = 0; j < TILE_COUNT_X; j++) {
    //         // convert to starting from bottom left
    //         int new_i = m_curmap.dimensions.y - TILE_COUNT_Y + i + 1;
    //         int new_j = j;

    //         auto tile = memnew(Tile(this, Vector2i(new_j, new_i)));
    //         tile->set_position(Vector2(j * TILE_SIZE + (TILE_SIZE / 2), i * TILE_SIZE + (TILE_SIZE / 2)));
    //         tile->set_process_priority(static_cast<int>(ProcessingPriority::Tiles));
    //         tile->set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Tiles));
    //         m_tiles_node->add_child(tile);
    //     }
    // }

    // // Setup player
    // auto player = memnew(Player(this, Vector2(0, 0)));
    // player->set_position(Vector2(0, 0));
    // player->set_process_priority(static_cast<int>(ProcessingPriority::Player));
    // player->set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Player));
    // m_mobs_node->add_child(player);

    // // Setup camera
    // int iy = static_cast<int>(roundl(m_camera->get_viewport_rect().get_size().y) / roundl(m_camera->get_zoom().y) / 2);
    // int ix = static_cast<int>(roundl(m_camera->get_viewport_rect().get_size().x) / roundl(m_camera->get_zoom().x) / 2);
    // m_camera_bounds = Rect2i{ix, iy, m_curmap.dimensions.x * TILE_SIZE - ix, m_curmap.dimensions.y * TILE_SIZE - iy};
    // m_camera_pos = Vector2{(float)(ix), (float)(m_curmap.dimensions.y * TILE_SIZE - iy)};
}

void Level::_physics_process(double delta) {
    // const int maxvel = 2;
    // m_camera_vel = Vector2(
    //     (Input::get_singleton()->is_action_pressed("ui_right") - Input::get_singleton()->is_action_pressed("ui_left")) * maxvel,
    //     (Input::get_singleton()->is_action_pressed("ui_down") - Input::get_singleton()->is_action_pressed("ui_up")) * maxvel
    // );

    // m_camera_pos += m_camera_vel;

    // if (m_camera_pos.x < m_camera_bounds.get_position().x) {
    //     m_camera_pos.x = 0;
    //     m_camera_vel.x = 0;
    // } else if (m_camera_pos.x > m_camera_bounds.get_end().x) {
    //     m_camera_pos.x = m_camera_bounds.size.x;
    //     m_camera_vel.x = 0;
    // }

    // if (m_camera_pos.y < m_camera_bounds.get_position().y) {
    //     m_camera_pos.y = 0;
    //     m_camera_vel.y = 0;
    // } else if (m_camera_pos.y > m_camera_bounds.get_end().y) {
    //     m_camera_pos.y = m_camera_bounds.size.y;
    //     m_camera_vel.y = 0;
    // }
}
