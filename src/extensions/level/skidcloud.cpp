#include "skidcloud.h"
#include "level.h"

using namespace godot;

constexpr char *SMOKE_FRAMES[] = {
    "Smoke1",
    "Smoke2"
};

void SkidCloud::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_pos"), &SkidCloud::get_pos);
    ClassDB::bind_method(D_METHOD("set_pos", "pos"), &SkidCloud::set_pos);
    ClassDB::add_property("SkidCloud", PropertyInfo(Variant::VECTOR2, "m_pos"), "set_pos", "get_pos");

    ClassDB::bind_method(D_METHOD("get_frame"), &SkidCloud::get_frame);
    ClassDB::bind_method(D_METHOD("set_frame", "frame"), &SkidCloud::set_frame);
    ClassDB::add_property("SkidCloud", PropertyInfo(Variant::VECTOR2, "m_frame"), "set_frame", "get_frame");
}

SkidCloud::SkidCloud() {
}

SkidCloud::SkidCloud(Level *level, Vector2 pos) : m_level(level), m_pos(pos) {
    m_frame = 0;
    /* make the smoke cloud appear at the player's feet */
    m_pos.y += 20;
    set_texture(m_level->m_player_preloader->get_resource(SMOKE_FRAMES[0]));
}

SkidCloud::~SkidCloud() {
}

void SkidCloud::_process(double delta) {
    set_position(m_pos - m_level->m_camera_pos);
    set_visible(true);
}

void SkidCloud::set_pos(const Vector2 pos) {
    m_pos = pos;
}

Vector2 SkidCloud::get_pos() const {
    return m_pos;
}

void SkidCloud::set_frame(int frame) {
    m_frame = frame;
}

int SkidCloud::get_frame() const {
    return m_frame;
}