#include "marker.h"
#include "level.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Marker::_bind_methods() {
}

Marker::Marker() {
}

Marker::Marker(Level *level, Vector2i true_pos, const Ref<Resource> texture) : m_level(level), m_true_pos(true_pos) {
    set_name("Start position marker");
    set_process_mode(ProcessMode::PROCESS_MODE_DISABLED);

    set_texture(texture);
    set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Tiles));
    set_z_index(static_cast<int>(ZIndex::Markers));
    set_self_modulate(Color{1, 1, 1, 0.75});
}

Marker::~Marker() {
}

void Marker::_process(double delta) {
}

void Marker::_physics_process(double delta) {
    const Rect2 viewport_rect{m_level->m_camera_true_pos.x - CAMERA_WIDTH / 2, m_level->m_camera_true_pos.y - CAMERA_HEIGHT / 2, CAMERA_WIDTH, CAMERA_HEIGHT};
    const Rect2 marker_rect{static_cast<float>(m_true_pos.x * TILE_SIZE), static_cast<float>(m_true_pos.y * TILE_SIZE), TILE_SIZE, TILE_SIZE};

    if (viewport_rect.intersects(marker_rect)) {
        set_position(marker_rect.position + Vector2{HALF_TILE, HALF_TILE} - m_level->m_camera_true_pos);
        set_visible(true);
    } else {
        set_visible(false);
    }
}
