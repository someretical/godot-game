#include "brush.h"
#include "level.h"
#include "tile_data.h"
#include "marker.h"

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/display_server.hpp>

using namespace godot;

void Brush::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_tile"), &Brush::get_tile);
	ClassDB::bind_method(D_METHOD("set_tile", "pos"), &Brush::set_tile);
    ClassDB::add_property("Brush", PropertyInfo(Variant::INT, "m_tile"), "set_tile", "get_tile");

    ClassDB::bind_method(D_METHOD("get_variant"), &Brush::get_variant);
	ClassDB::bind_method(D_METHOD("set_variant", "pos"), &Brush::set_variant);
    ClassDB::add_property("Brush", PropertyInfo(Variant::INT, "m_variant"), "set_variant", "get_variant");

    ClassDB::bind_method(D_METHOD("handle_export_callback"), &Brush::handle_export_callback);
    ClassDB::bind_method(D_METHOD("handle_import_callback"), &Brush::handle_import_callback);
}

Brush::Brush() {
}

Brush::Brush(Level *level) : m_level(level) {
    m_tile_group = 0;
    m_variant = 0;

    set_name("Level editor brush");
    set_process_mode(ProcessMode::PROCESS_MODE_DISABLED);

    set_process_priority(static_cast<int>(ProcessingPriority::Tiles));
    set_z_index(static_cast<int>(ZIndex::EditorBrush));

    set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile_group][m_variant].data()));
    set_self_modulate(Color(1, 1, 1, 0.5));
}

Brush::~Brush() {
}

void Brush::_ready() {
}

void Brush::_process(double delta) {
    const auto &campos = m_level->m_camera_true_pos;
    const auto mouse_pos = get_viewport()->get_mouse_position();
    const auto grid_pos = Vector2i(
        static_cast<int>((mouse_pos.x + campos.x - (CAMERA_WIDTH / 2)) / TILE_SIZE),
        static_cast<int>((mouse_pos.y + campos.y - (CAMERA_HEIGHT / 2)) / TILE_SIZE)
    );
    const auto screen_pos = Vector2(grid_pos.x * TILE_SIZE + HALF_TILE, grid_pos.y * TILE_SIZE + HALF_TILE) - campos;

    set_position(screen_pos);
}

void Brush::_physics_process(double delta) {
    if (Input::get_singleton()->is_mouse_button_pressed(MouseButton::MOUSE_BUTTON_LEFT)) {
        const auto grid_pos = get_grid_pos(get_viewport()->get_mouse_position());
        if (grid_pos.x == -1 || grid_pos.y == -1) {
            return;
        }

        auto &existing = m_level->m_curmap->m_tile_data[grid_pos.y][grid_pos.x];
        const auto max_size = TileData::get_variant_sizes()[m_tile_group];
        const auto variant_index = m_variant >= max_size ? max_size - 1 : m_variant;

        if (m_tile_group == 6) {
            /* handle start position tile differently */
            m_level->m_curmap->m_start_pos = grid_pos;
            m_level->m_editor.m_start_pos->m_true_pos = grid_pos;
        } else {
            existing.m_tile_group = m_tile_group;
            existing.m_variant = variant_index;
        }
    } else if (Input::get_singleton()->is_mouse_button_pressed(MouseButton::MOUSE_BUTTON_RIGHT)) {
        const auto grid_pos = get_grid_pos(get_viewport()->get_mouse_position());
        if (grid_pos.x == -1 || grid_pos.y == -1) {
            return;
        }

        auto &existing = m_level->m_curmap->m_tile_data[grid_pos.y][grid_pos.x];
        existing.m_tile_group = -1;
        existing.m_variant = -1;
    }
}

Vector2i Brush::get_grid_pos(const Vector2 pos) const {
    const Vector2i grid_pos{
        static_cast<int>((pos.x + m_level->m_camera_true_pos.x - (CAMERA_WIDTH / 2)) / TILE_SIZE),
        static_cast<int>((pos.y + m_level->m_camera_true_pos.y - (CAMERA_HEIGHT / 2)) / TILE_SIZE)
    };

    if (
        grid_pos.x < 0 || grid_pos.y < 0 || 
        grid_pos.x >= m_level->m_curmap->m_dimensions.x || 
        grid_pos.y >= m_level->m_curmap->m_dimensions.y
    ) {
        return Vector2i(-1, -1);
    }

    return grid_pos;
};

void Brush::_unhandled_input(const Ref<InputEvent> &event) {
    if (handle_export_level(event)) return;
    if (handle_import_level(event)) return;
    if (handle_next_tile_variant(event)) return;
    if (handle_next_tile_group(event)) return;
    if (handle_pick_tile(event)) return;
}

bool Brush::handle_next_tile_group(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_editor_next_tile_group")) {
        m_tile_group++;
        if (m_tile_group >= TileData::get_variant_sizes().size()) {
            m_tile_group = 0;
        }

        const auto max_size = TileData::get_variant_sizes()[m_tile_group];
        const auto variant_index = m_variant >= max_size ? max_size - 1 : m_variant;
        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile_group][variant_index].data()));
        get_viewport()->set_input_as_handled();
        return true;
    } else if (event->is_action_pressed("ui_editor_prev_tile_group")) {
        m_tile_group--;
        if (m_tile_group < 0) {
            m_tile_group = TileData::get_variant_sizes().size() - 1;
        }

        const auto max_size = TileData::get_variant_sizes()[m_tile_group];
        const auto variant_index = m_variant >= max_size ? max_size - 1 : m_variant;
        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile_group][variant_index].data()));
        get_viewport()->set_input_as_handled();
        return true;
    }

    return false;
}

bool Brush::handle_next_tile_variant(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_editor_next_tile_variant")) {
        m_variant++;
        if (m_variant >= TileData::get_variant_sizes()[m_tile_group]) {
            m_variant = 0;
        }

        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile_group][m_variant].data()));
        get_viewport()->set_input_as_handled();
        return true;
    } else if (event->is_action_pressed("ui_editor_prev_tile_variant")) {
        m_variant--;
        if (m_variant < 0) {
            m_variant = TileData::get_variant_sizes()[m_tile_group] - 1;
        }

        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile_group][m_variant].data()));
        get_viewport()->set_input_as_handled();
        return true;
    }

    return false;
}

bool Brush::handle_pick_tile(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_editor_pick_tile")) {
        const auto grid_pos = get_grid_pos(get_viewport()->get_mouse_position());
        if (grid_pos.x == -1 || grid_pos.y == -1) {
            get_viewport()->set_input_as_handled();
            return true;
        }

        auto &existing = m_level->m_curmap->m_tile_data[grid_pos.y][grid_pos.x];
        if (existing.m_tile_group == -1 || existing.m_variant == -1) {
            get_viewport()->set_input_as_handled();
            return true;
        }

        m_tile_group = existing.m_tile_group;
        m_variant = existing.m_variant;
        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile_group][m_variant].data()));
        get_viewport()->set_input_as_handled();
        return true;
    }

    return false;
}

bool Brush::handle_export_level(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_editor_export_level_as")) {
    save_as_anyway:
        const auto error = DisplayServer::get_singleton()->file_dialog_show(
            "Save Level", 
            ".", 
            "level.json", 
            true, 
            DisplayServer::FileDialogMode::FILE_DIALOG_MODE_SAVE_FILE, 
            PackedStringArray{"*.json"}, 
            Callable(this, "handle_export_callback")
        );

        if (error != Error::OK) {
            UtilityFunctions::print("file_dialog_show error: ", error);
        }

        get_viewport()->set_input_as_handled();
        return true;
    } else if (event->is_action_pressed("ui_editor_export_level")) {
        if (m_level->m_curmap->m_path.is_empty() || m_level->export_current_map(m_level->m_curmap->m_path) != OK) {
            goto save_as_anyway;
        }

        get_viewport()->set_input_as_handled();
        return true;
    }

    return false;
}

void Brush::handle_export_callback(bool status, PackedStringArray paths, int selected_filter_index) {
    if (!status) {
        return;
    }

    m_level->export_current_map(paths[0]);
}

bool Brush::handle_import_level(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_editor_import_level")) {
        const auto error = DisplayServer::get_singleton()->file_dialog_show(
            "Import Level", 
            ".", 
            "", 
            true, 
            DisplayServer::FileDialogMode::FILE_DIALOG_MODE_OPEN_FILE, 
            PackedStringArray{"*.json"}, 
            Callable(this, "handle_import_callback")
        );

        if (error != Error::OK) {
            UtilityFunctions::print("file_dialog_show error: ", error);
        }

        get_viewport()->set_input_as_handled();
        return true;
    }

    return false;
}

void Brush::handle_import_callback(bool status, PackedStringArray paths, int selected_filter_index) {
    if (!status) {
        return;
    }

    const auto err = m_level->load_level(paths[0]);
    if (err != Error::OK) {
        UtilityFunctions::print("load_level error: ", err);
    }
}

void Brush::set_tile(const int tile) {
    m_tile_group = tile;
}

int Brush::get_tile() const {
    return m_tile_group;
}

void Brush::set_variant(const int variant) {
    m_variant = variant;
}

int Brush::get_variant() const {
    return m_variant;
}
