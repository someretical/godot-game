#include "skidcloud.h"
#include "level.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

constexpr std::array SMOKE_FRAMES = {
    "Smoke1",
    "Smoke2"
};

void SkidCloud::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_pos"), &SkidCloud::get_pos);
    ClassDB::bind_method(D_METHOD("set_pos", "pos"), &SkidCloud::set_pos);
    ClassDB::add_property("SkidCloud", PropertyInfo(Variant::VECTOR2, "m_pos"), "set_pos", "get_pos");
}

SkidCloud::SkidCloud() {
}

SkidCloud::SkidCloud(Level *level, Vector2 pos) : m_level(level), m_pos(pos) {
    /* make the smoke cloud appear at the player's feet */
    m_pos.y += 20;

    set_process_priority(static_cast<int>(ProcessingPriority::SkidCloud));
    set_z_index(static_cast<int>(ZIndex::SkidCloud));

    set_sprite_frames(ResourceLoader::get_singleton()->load("src/assets/particles/smoke.tres"));
    set_animation("default");
}

SkidCloud::~SkidCloud() {
}

void SkidCloud::_ready() {
    play();
    set_visible(true);
}

void SkidCloud::_process(double delta) {
    set_position(m_pos - m_level->m_camera_true_pos);

    if (get_frame() == 2) {
        set_visible(false);
        queue_free();
    }
}

void SkidCloud::set_pos(const Vector2 pos) {
    m_pos = pos;
}

Vector2 SkidCloud::get_pos() const {
    return m_pos;
}
