#include "brush.h"
#include "level.h"
#include "tile_data.h"

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

using namespace godot;

void Brush::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_tile"), &Brush::get_tile);
	ClassDB::bind_method(D_METHOD("set_tile", "pos"), &Brush::set_tile);
    ClassDB::add_property("Brush", PropertyInfo(Variant::INT, "m_tile"), "set_tile", "get_tile");

    ClassDB::bind_method(D_METHOD("get_variant"), &Brush::get_variant);
	ClassDB::bind_method(D_METHOD("set_variant", "pos"), &Brush::set_variant);
    ClassDB::add_property("Brush", PropertyInfo(Variant::INT, "m_variant"), "set_variant", "get_variant");
}

Brush::Brush() {
}

Brush::Brush(Level *level) : m_level(level) {
    m_tile = 0;
    m_variant = 0;
    set_process_priority(static_cast<int>(ProcessingPriority::Tiles));
    set_z_index(static_cast<int>(ZIndex::EditorBrush));

    set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile][m_variant].data()));
    set_self_modulate(Color(1, 1, 1, 0.5));
    set_visible(true);
}

Brush::~Brush() {
}

void Brush::_ready() {
}

void Brush::_process(double delta) {
    const auto &campos = m_level->m_camera_pos;
    const auto mouse_pos = get_viewport()->get_mouse_position();
    const auto grid_pos = Vector2i(
        static_cast<int>((mouse_pos.x + campos.x - (CAMERA_WIDTH / 2)) / TILE_SIZE),
        static_cast<int>((mouse_pos.y + campos.y - (CAMERA_HEIGHT / 2)) / TILE_SIZE)
    );
    const auto screen_pos = Vector2(grid_pos.x * TILE_SIZE + HALF_TILE, grid_pos.y * TILE_SIZE + HALF_TILE) - campos;

    set_position(screen_pos);
}

void Brush::_unhandled_input(const Ref<InputEvent> &event) {
    handle_next_tile_group(event);
    handle_next_tile_variant(event);
    handle_place_tile(event);
}

void Brush::handle_next_tile_group(const Ref<InputEvent> &event) {
    const auto key_event = Object::cast_to<const InputEventKey>(*event);
    if (!key_event) {
        return;
    }

    static auto ignore_next_e_press = false;
    static auto ignore_next_e_release = false;
    if (key_event->is_pressed() && key_event->get_keycode() == KEY_E && !ignore_next_e_press) {
        ignore_next_e_press = true;
        ignore_next_e_release = false;

        m_tile++;
        if (m_tile >= TileData::get_variant_sizes().size()) {
            m_tile = 0;
        }

        const auto max_size = TileData::get_variant_sizes()[m_tile];
        const auto variant_index = m_variant >= max_size ? max_size - 1 : m_variant;
        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile][variant_index].data()));
    }

    if (key_event->is_released() && key_event->get_keycode() == KEY_E && !ignore_next_e_release) {
        ignore_next_e_press = false;
        ignore_next_e_release = true;
    }
}

void Brush::handle_next_tile_variant(const Ref<InputEvent> &event) {
    const auto key_event = Object::cast_to<const InputEventKey>(*event);
    if (!key_event) {
        return;
    }

    static auto ignore_next_r_press = false;
    static auto ignore_next_r_release = false;
    if (key_event->is_pressed() && key_event->get_keycode() == KEY_R && !ignore_next_r_press) {
        ignore_next_r_press = true;
        ignore_next_r_release = false;

        m_variant++;
        if (m_variant >= TileData::get_variant_sizes()[m_tile]) {
            m_variant = 0;
        }

        set_texture(m_level->m_tile_preloader->get_resource(TileData::get_tile_variants()[m_tile][m_variant].data()));
    }

    if (key_event->is_released() && key_event->get_keycode() == KEY_R && !ignore_next_r_release) {
        ignore_next_r_press = false;
        ignore_next_r_release = true;
    }
}

void Brush::handle_place_tile(const Ref<InputEvent> &event) {
    const auto mouse_event = Object::cast_to<const InputEventMouseButton>(*event);
    if (!mouse_event) {
        return;
    }

    static auto ignore_next_m1_press = false;
    static auto ignore_next_m1_release = false;
    if (mouse_event->is_pressed() && mouse_event->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && !ignore_next_m1_press) {
        ignore_next_m1_press = true;
        ignore_next_m1_release = false;

        const auto mouse_pos = get_viewport()->get_mouse_position();
        const auto grid_pos = Vector2i(
            static_cast<int>((mouse_pos.x + m_level->m_camera_pos.x - (CAMERA_WIDTH / 2)) / TILE_SIZE),
            static_cast<int>((mouse_pos.y + m_level->m_camera_pos.y - (CAMERA_HEIGHT / 2)) / TILE_SIZE)
        );

        if (
            grid_pos.x < 0 || grid_pos.y < 0 || 
            grid_pos.x >= m_level->m_curmap.m_dimensions.x || 
            grid_pos.y >= m_level->m_curmap.m_dimensions.y
        ) {
            return;
        }

        auto &existing = m_level->m_curmap.tile_data[grid_pos.y][grid_pos.x];
        if (existing.m_tile_group == m_tile && existing.m_variant == m_variant) {
            existing.m_tile_group = -1;
            existing.m_variant = -1;
        } else {
            existing.m_tile_group = m_tile;
            existing.m_variant = m_variant;
        }
    }

    if (mouse_event->is_released() && mouse_event->get_button_index() == MouseButton::MOUSE_BUTTON_LEFT && !ignore_next_m1_release) {
        ignore_next_m1_press = false;
        ignore_next_m1_release = true;
    }
}

void Brush::set_tile(const int tile) {
    m_tile = tile;
}

int Brush::get_tile() const {
    return m_tile;
}

void Brush::set_variant(const int variant) {
    m_variant = variant;
}

int Brush::get_variant() const {
    return m_variant;
}
