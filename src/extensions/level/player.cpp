#include "level.h"
#include "player.h"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_pos"), &Player::get_pos);
	ClassDB::bind_method(D_METHOD("set_pos", "pos"), &Player::set_pos);
    ClassDB::add_property("Player", PropertyInfo(Variant::VECTOR2, "m_pos"), "set_pos", "get_pos");

    ClassDB::bind_method(D_METHOD("get_vel"), &Player::get_vel);
	ClassDB::bind_method(D_METHOD("set_vel", "vel"), &Player::set_vel);
    ClassDB::add_property("Player", PropertyInfo(Variant::VECTOR2, "m_vel"), "set_vel", "get_vel");
}

Player::Player() {
}

Player::Player(Level *level, Vector2 pos) : m_level(level), m_pos(pos) {
    set_texture(m_level->m_tile_preloader->get_resource(m_level->m_tile_preloader->get_resource_list()[3]));
}

Player::~Player() {
}

void Player::_process(double delta) {
}

void Player::_physics_process(double delta) {
    const int maxvel = 3;
    auto d = Vector2(
        (Input::get_singleton()->is_action_pressed("ui_right") - Input::get_singleton()->is_action_pressed("ui_left")) * maxvel,
        (Input::get_singleton()->is_action_pressed("ui_down") - Input::get_singleton()->is_action_pressed("ui_up")) * maxvel
    );

    /* update player location in level */
    m_pos += d;

    /* update camera location in level */
    auto &cam_pos = m_level->m_camera_pos;
    cam_pos.x = m_pos.x;
    /* vertical movement of camera lags behind player position to prevent jerkiness */
    cam_pos.y += (m_pos.y - cam_pos.y) / 4;

    if (cam_pos.x < CAMERA_WIDTH / 2) {
        cam_pos.x = CAMERA_WIDTH / 2;
    } else if (cam_pos.x > m_level->m_curmap.dimensions.x * TILE_SIZE - CAMERA_WIDTH / 2) {
        cam_pos.x = m_level->m_curmap.dimensions.x * TILE_SIZE - CAMERA_WIDTH / 2;
    }

    if (cam_pos.y < CAMERA_HEIGHT / 2) {
        cam_pos.y = CAMERA_HEIGHT / 2;
    } else if (cam_pos.y > m_level->m_curmap.dimensions.y * TILE_SIZE - CAMERA_HEIGHT / 2) {
        cam_pos.y = m_level->m_curmap.dimensions.y * TILE_SIZE - CAMERA_HEIGHT / 2;
    }

    /* update where player is located on screen */
    set_position(m_pos - cam_pos);
}

void Player::set_pos(const Vector2 pos) {
    m_pos = pos;
}

Vector2 Player::get_pos() const {
    return m_pos;
}

void Player::set_vel(const Vector2 vel) {
    m_vel = vel;
}

Vector2 Player::get_vel() const {
    return m_vel;
}