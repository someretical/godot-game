#include "player.h"
#include "level.h"
#include "tile.h"

#include <cmath>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void Level::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_camera_pos"), &Level::get_camera_pos);
	ClassDB::bind_method(D_METHOD("set_camera_pos", "pos"), &Level::set_camera_pos);
    ClassDB::add_property("Level", PropertyInfo(Variant::VECTOR2, "m_camera_pos"), "set_camera_pos", "get_camera_pos");
}

Level::Level() {
    m_rng = memnew(RandomNumberGenerator);
    m_rng->randomize();

    const auto loader = ResourceLoader::get_singleton();

    m_player_preloader = memnew(ResourcePreloader);
    m_player_preloader->add_resource("player_hitbox", loader->load("res://src/assets/player/player_hitbox.png"));
    add_child(m_player_preloader);

    m_tile_preloader = memnew(ResourcePreloader);
    m_tile_preloader->add_resource("beacon", loader->load("res://src/assets/tiles/beacon.png"));
    m_tile_preloader->add_resource("bedrock", loader->load("res://src/assets/tiles/bedrock.png"));
    m_tile_preloader->add_resource("bee_nest_front", loader->load("res://src/assets/tiles/bee_nest_front.png"));
    m_tile_preloader->add_resource("blackstone", loader->load("res://src/assets/tiles/blackstone.png"));
    m_tile_preloader->add_resource("cobblestone", loader->load("res://src/assets/tiles/cobblestone.png"));
    m_tile_preloader->add_resource("cobweb", loader->load("res://src/assets/tiles/cobweb.png"));
    m_tile_preloader->add_resource("dirt", loader->load("res://src/assets/tiles/dirt.png"));
    m_tile_preloader->add_resource("grass_block_side", loader->load("res://src/assets/tiles/grass_block_side.png"));
    m_tile_preloader->add_resource("red_sand", loader->load("res://src/assets/tiles/red_sand.png"));
    m_tile_preloader->add_resource("sand", loader->load("res://src/assets/tiles/sand.png"));
    m_tile_preloader->add_resource("smooth_stone", loader->load("res://src/assets/tiles/smooth_stone.png"));
    add_child(m_tile_preloader);

    m_map_node = memnew(Node);
    add_child(m_map_node);

    m_tiles_node = memnew(Node);
    m_map_node->add_child(m_tiles_node);

    m_collectables_node = memnew(Node);
    m_map_node->add_child(m_collectables_node);

    m_mobs_node = memnew(Node);
    m_map_node->add_child(m_mobs_node);

    // Create in memory map
    m_curmap.dimensions = Vector2{100, 20};
    m_curmap.tile_data = memnew_arr(int *, m_curmap.dimensions.y);
    for (int j = 0; j < m_curmap.dimensions.y; j++) {
        m_curmap.tile_data[j] = memnew_arr(int, m_curmap.dimensions.x);
    }

    for (int j = 1; j < m_curmap.dimensions.y - 1; j++) {
        for (int i = 1; i < m_curmap.dimensions.x - 1; i++) {
            m_curmap.tile_data[j][i] = m_rng->randi_range(0, 25) == 0 ? m_rng->randi_range(0, 10) : -1;
            // m_curmap.tile_data[j][i] = -1;
        }
    }

    for (int j = 0; j < m_curmap.dimensions.y; j++) {
        m_curmap.tile_data[j][0] = 1;
        m_curmap.tile_data[j][m_curmap.dimensions.x - 1] = 1;
    }

    for (int j = 0; j < m_curmap.dimensions.x; j++) {
        m_curmap.tile_data[0][j] = 1;
        m_curmap.tile_data[m_curmap.dimensions.y - 1][j] = 1;
    }

    int i = 0;
    for (int x = HALF_TILE; x < TILE_COUNT_X * TILE_SIZE + HALF_TILE; x += TILE_SIZE, i++) {
        int j = 0;
        for (int y = HALF_TILE; y < TILE_COUNT_Y * TILE_SIZE + HALF_TILE; y += TILE_SIZE, j++) {
            auto tile = memnew(Tile(this, Vector2(x, y), Vector2i(i, j)));
            tile->set_process_priority(static_cast<int>(ProcessingPriority::Tiles));
            tile->set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Tiles));
            m_tiles_node->add_child(tile);
        }
    }

    m_bounds = Rect2i{0, 0, m_curmap.dimensions.x * TILE_SIZE, m_curmap.dimensions.y * TILE_SIZE};

    // Setup player
    m_player = memnew(Player(this, Vector2{CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2}));
    m_player->set_process_priority(static_cast<int>(ProcessingPriority::Player));
    m_player->set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Player));
    m_mobs_node->add_child(m_player);

    // Setup camera
    m_camera = memnew(Camera2D);
    m_camera->set_zoom(Vector2(SCREEN_ZOOM, SCREEN_ZOOM));
    add_child(m_camera);
    m_camera_pos = Vector2{CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2};
}

Level::~Level() {
}

void Level::set_camera_pos(const Vector2 pos) {
    m_camera_pos = pos;
}

Vector2 Level::get_camera_pos() const {
    return m_camera_pos;
}

void Level::update_camera() {
    /* update camera location in level */
    m_camera_pos.x = m_player->m_pos.x;
    /* vertical movement of camera lags behind player position to prevent jerkiness */
    m_camera_pos.y += (m_player->m_pos.y - m_camera_pos.y) / 4;

    if (m_camera_pos.x < CAMERA_WIDTH / 2) {
        m_camera_pos.x = CAMERA_WIDTH / 2;
    } else if (m_camera_pos.x > m_curmap.dimensions.x * TILE_SIZE - CAMERA_WIDTH / 2) {
        m_camera_pos.x = m_curmap.dimensions.x * TILE_SIZE - CAMERA_WIDTH / 2;
    }

    if (m_camera_pos.y < CAMERA_HEIGHT / 2) {
        m_camera_pos.y = CAMERA_HEIGHT / 2;
    } else if (m_camera_pos.y > m_curmap.dimensions.y * TILE_SIZE - CAMERA_HEIGHT / 2) {
        m_camera_pos.y = m_curmap.dimensions.y * TILE_SIZE - CAMERA_HEIGHT / 2;
    }
}

void Level::_ready() {
    // // Finish setting up camera
    // const int cam_width = static_cast<int>(m_camera->get_viewport_rect().get_size().x / m_camera->get_zoom().x);
    // const int cam_height = static_cast<int>(m_camera->get_viewport_rect().get_size().y / m_camera->get_zoom().y);
    // const int map_width = m_curmap.dimensions.x * TILE_SIZE;
    // const int map_height = m_curmap.dimensions.y * TILE_SIZE;
    // // Last 2 arguments for Rect2i are the width and height, not the coords of the bottom right corner
    // m_camera_bounds = Rect2i{cam_width / 2, cam_height / 2, map_width - cam_width, map_height - cam_height};
    // m_camera_pos = Vector2{(float)m_camera_bounds.get_position().x, (float)m_camera_bounds.get_end().y};
    // m_camera->set_offset(m_camera_bounds.get_position());

    // /* where player is located in the level */
    // m_player->m_pos = m_camera_pos;
    // /* where player is located on screen */
    // m_player->set_position(Vector2(cam_width / 2, cam_height / 2));
}

void Level::_physics_process(double delta) {
}
