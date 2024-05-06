#include "player.h"
#include "level.h"
#include "tile.h"
#include "brush.h"
#include "tile_data.h"
#include "mapdata.h"

#include <cmath>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void Level::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_camera_pos"), &Level::get_camera_pos);
	ClassDB::bind_method(D_METHOD("set_camera_pos", "pos"), &Level::set_camera_pos);
    ClassDB::add_property("Level", PropertyInfo(Variant::VECTOR2, "m_camera_pos"), "set_camera_pos", "get_camera_pos");
}

Level::Level() {
    /* prevent process from running in background editor */
    if (Engine::get_singleton()->is_editor_hint()) {
        set_process_mode(ProcessMode::PROCESS_MODE_DISABLED);
    }

    m_rng = memnew(RandomNumberGenerator);
    m_rng->randomize();

    const auto loader = ResourceLoader::get_singleton();

    m_tile_preloader = memnew(ResourcePreloader);
    m_tile_preloader->set_name("Tile Preloader");
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
    m_tile_preloader->add_resource("Coin1", loader->load("src/assets/entities/coin/coin1.png"));
    m_tile_preloader->add_resource("Coin2", loader->load("src/assets/entities/coin/coin2.png"));
    m_tile_preloader->add_resource("Coin3", loader->load("src/assets/entities/coin/coin3.png"));
    m_tile_preloader->add_resource("Coin4", loader->load("src/assets/entities/coin/coin4.png"));
    m_tile_preloader->add_resource("Question1", loader->load("src/assets/tiles/question/Question1.png"));
    m_tile_preloader->add_resource("Question2", loader->load("src/assets/tiles/question/Question2.png"));
    m_tile_preloader->add_resource("Question3", loader->load("src/assets/tiles/question/Question3.png"));
    m_tile_preloader->add_resource("Question4", loader->load("src/assets/tiles/question/Question4.png"));
    add_child(m_tile_preloader);

    m_map_node = memnew(Node);
    m_map_node->set_name("Map");
    add_child(m_map_node);

    m_tiles_node = memnew(Node);
    m_tiles_node->set_name("Tiles");
    m_map_node->add_child(m_tiles_node);

    m_collectables_node = memnew(Node);
    m_collectables_node->set_name("Collectables");
    m_map_node->add_child(m_collectables_node);

    m_mobs_node = memnew(Node);
    m_mobs_node->set_name("Mobs");
    m_map_node->add_child(m_mobs_node);

    m_particles_node = memnew(Node);
    m_particles_node->set_name("Particles");
    m_map_node->add_child(m_particles_node);

    m_editor.m_brush = memnew(Brush(this));
    m_editor.m_brush->set_name("Brush");
    m_editor.m_enabled = false;

    if (auto map = MapData::load_bare_map(); map.has_value()) {
        /* interesting fuckery because unique_ptr members are normally supposed to be initialized in the initializer list */
        m_curmap.reset(std::move(map.value().release()));
    } else {
        UtilityFunctions::print("Failed to load map");
        std::exit(1);
    }

    int i = 0;
    for (int x = HALF_TILE; x < TILE_COUNT_X * TILE_SIZE + HALF_TILE; x += TILE_SIZE, i++) {
        int j = 0;
        for (int y = HALF_TILE; y < TILE_COUNT_Y * TILE_SIZE + HALF_TILE; y += TILE_SIZE, j++) {
            auto tile = memnew(Tile(this, Vector2(x, y), Vector2i(i, j)));
            m_tiles_node->add_child(tile);
        }
    }

    m_bounds = Rect2{
        0 + TINY, 
        0 + TINY, 
        static_cast<float>(m_curmap->m_dimensions.x * TILE_SIZE) - (2 * TINY), 
        static_cast<float>(m_curmap->m_dimensions.y * TILE_SIZE) - (2 * TINY)
    };

    /* Setup player */
    m_player = memnew(Player(this, Vector2{CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2}));
    m_mobs_node->add_child(m_player);

    /* Setup camera */
    m_camera = memnew(Camera2D);
    m_camera->set_zoom(Vector2(SCREEN_ZOOM, SCREEN_ZOOM));
    add_child(m_camera);
    m_camera_pos = Vector2{CAMERA_WIDTH / 2, CAMERA_HEIGHT / 2};
}

Level::~Level() {
    memdelete(m_rng);
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
    } else if (m_camera_pos.x > m_curmap->m_dimensions.x * TILE_SIZE - CAMERA_WIDTH / 2) {
        m_camera_pos.x = m_curmap->m_dimensions.x * TILE_SIZE - CAMERA_WIDTH / 2;
    }

    if (m_camera_pos.y < CAMERA_HEIGHT / 2) {
        m_camera_pos.y = CAMERA_HEIGHT / 2;
    } else if (m_camera_pos.y > m_curmap->m_dimensions.y * TILE_SIZE - CAMERA_HEIGHT / 2) {
        m_camera_pos.y = m_curmap->m_dimensions.y * TILE_SIZE - CAMERA_HEIGHT / 2;
    }
}

Error Level::import_map_inplace(const String &path) {
    if (auto map = MapData::load_map(path); map.has_value()) {
        m_curmap.reset(std::move(map.value().release()));
        return OK;
    } else {
        return map.error();
    }
}

Error Level::export_current_map(const String &path) {
    return m_curmap->save_map(path);
}


void Level::_input(const Ref<InputEvent> &event) {
    handle_editor_input(event);
}

void Level::handle_editor_input(const Ref<InputEvent> &event) {
    static auto ignore_next_f1_press = false;
    static auto ignore_next_f1_release = false;
    const auto key_event = Object::cast_to<const InputEventKey>(*event);

    if (!key_event) return;

    if (key_event->is_pressed() && key_event->get_keycode() == KEY_F1 && !ignore_next_f1_press) {
        ignore_next_f1_press = true;
        ignore_next_f1_release = false;

        m_editor.m_enabled = !m_editor.m_enabled;
        if (m_editor.m_enabled) {
            add_child(m_editor.m_brush);
            UtilityFunctions::print("Editor enabled");
        } else {
            remove_child(m_editor.m_brush);
            UtilityFunctions::print("Editor disabled");
        }
    }

    if (key_event->is_released() && key_event->get_keycode() == KEY_F1 && !ignore_next_f1_release) {
        ignore_next_f1_press = false;
        ignore_next_f1_release = true;
    }
}

void Level::_ready() {
}
