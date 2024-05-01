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
    m_player_preloader->add_resource("MarioCrouch", loader->load("src/assets/player/MarioCrouch.png"));
    m_player_preloader->add_resource("MarioDeath", loader->load("src/assets/player/MarioDeath.png"));
    m_player_preloader->add_resource("MarioJump", loader->load("src/assets/player/MarioJump.png"));
    m_player_preloader->add_resource("MarioSlide", loader->load("src/assets/player/MarioSlide.png"));
    m_player_preloader->add_resource("MarioTurn", loader->load("src/assets/player/MarioTurn.png"));
    m_player_preloader->add_resource("MarioWalk1", loader->load("src/assets/player/MarioWalk1.png"));
    m_player_preloader->add_resource("MarioWalk2", loader->load("src/assets/player/MarioWalk2.png"));
    m_player_preloader->add_resource("MarioWalk3", loader->load("src/assets/player/MarioWalk3.png"));
    m_player_preloader->add_resource("MarioWalk4", loader->load("src/assets/player/MarioWalk4.png"));
    m_player_preloader->add_resource("Smoke1", loader->load("src/assets/particles/Smoke1.png"));
    m_player_preloader->add_resource("Smoke2", loader->load("src/assets/particles/Smoke2.png"));
    add_child(m_player_preloader);

    m_tile_preloader = memnew(ResourcePreloader);
    m_tile_preloader->add_resource("Blue-1", loader->load("src/assets/tiles/blue/Blue-1.png"));
    m_tile_preloader->add_resource("Blue-2", loader->load("src/assets/tiles/blue/Blue-2.png"));
    m_tile_preloader->add_resource("Blue-3", loader->load("src/assets/tiles/blue/Blue-3.png"));
    m_tile_preloader->add_resource("Blue-4", loader->load("src/assets/tiles/blue/Blue-4.png"));
    m_tile_preloader->add_resource("Blue-5", loader->load("src/assets/tiles/blue/Blue-5.png"));
    m_tile_preloader->add_resource("Blue-6", loader->load("src/assets/tiles/blue/Blue-6.png"));
    m_tile_preloader->add_resource("Blue-7", loader->load("src/assets/tiles/blue/Blue-7.png"));
    m_tile_preloader->add_resource("Blue-8", loader->load("src/assets/tiles/blue/Blue-8.png"));
    m_tile_preloader->add_resource("Blue-9", loader->load("src/assets/tiles/blue/Blue-9.png"));
    m_tile_preloader->add_resource("Block-Gold", loader->load("src/assets/tiles/Block-Gold.png"));
    m_tile_preloader->add_resource("Block-Wood", loader->load("src/assets/tiles/wood/Block-Wood.png"));
    m_tile_preloader->add_resource("Wood-0", loader->load("src/assets/tiles/wood/Wood-0.png"));
    m_tile_preloader->add_resource("Wood-1", loader->load("src/assets/tiles/wood/Wood-1.png"));
    m_tile_preloader->add_resource("Wood-2", loader->load("src/assets/tiles/wood/Wood-2.png"));
    m_tile_preloader->add_resource("Wood-3", loader->load("src/assets/tiles/wood/Wood-3.png"));
    m_tile_preloader->add_resource("Wood-4", loader->load("src/assets/tiles/wood/Wood-4.png"));
    m_tile_preloader->add_resource("Wood-5", loader->load("src/assets/tiles/wood/Wood-5.png"));
    add_child(m_tile_preloader);

    // add_resource("Coin1", loader->load("src/assets/entities/coin/Coin1.png"));
    // add_resource("Coin2", loader->load("src/assets/entities/coin/Coin2.png"));
    // add_resource("Coin3", loader->load("src/assets/entities/coin/Coin3.png"));
    // add_resource("Coin4", loader->load("src/assets/entities/coin/Coin4.png"));
    // add_resource("Question1", loader->load("src/assets/tiles/question/Question1.png"));
    // add_resource("Question2", loader->load("src/assets/tiles/question/Question2.png"));
    // add_resource("Question3", loader->load("src/assets/tiles/question/Question3.png"));
    // add_resource("Question4", loader->load("src/assets/tiles/question/Question4.png"));

    m_map_node = memnew(Node);
    add_child(m_map_node);

    m_tiles_node = memnew(Node);
    m_map_node->add_child(m_tiles_node);

    m_collectables_node = memnew(Node);
    m_map_node->add_child(m_collectables_node);

    m_mobs_node = memnew(Node);
    m_map_node->add_child(m_mobs_node);

    m_particles_node = memnew(Node);
    m_map_node->add_child(m_particles_node);

    // Create in memory map
    m_curmap.dimensions = Vector2{100, 20};
    m_curmap.tile_data = memnew_arr(int *, m_curmap.dimensions.y);
    for (int j = 0; j < m_curmap.dimensions.y; j++) {
        m_curmap.tile_data[j] = memnew_arr(int, m_curmap.dimensions.x);
    }

    for (int j = 1; j < m_curmap.dimensions.y - 1; j++) {
        for (int i = 1; i < m_curmap.dimensions.x - 1; i++) {
            m_curmap.tile_data[j][i] = m_rng->randi_range(0, 25) == 0 ? 9 : -1;
            // m_curmap.tile_data[j][i] = -1;
        }
    }

    for (int j = 0; j < m_curmap.dimensions.y; j++) {
        m_curmap.tile_data[j][0] = 10;
        m_curmap.tile_data[j][m_curmap.dimensions.x - 1] = 10;
    }

    for (int j = 0; j < m_curmap.dimensions.x; j++) {
        m_curmap.tile_data[0][j] = 10;
        m_curmap.tile_data[m_curmap.dimensions.y - 1][j] = 10;
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
    memdelete(m_rng);
    for (int j = 0; j < m_curmap.dimensions.y; j++) {
        memdelete_arr(m_curmap.tile_data[j]);
    }
    memdelete_arr(m_curmap.tile_data);
    queue_free();
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
}

void Level::_physics_process(double delta) {
}
