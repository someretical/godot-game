#include "level.h"
#include "player.h"

#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_pos"), &Player::get_pos);
	ClassDB::bind_method(D_METHOD("set_pos", "pos"), &Player::set_pos);
    ClassDB::add_property("Player", PropertyInfo(Variant::VECTOR2, "m_camera_pos"), "set_pos", "get_pos");
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
    const int maxvel = 2;
    auto d = Vector2(
        (Input::get_singleton()->is_action_pressed("ui_right") - Input::get_singleton()->is_action_pressed("ui_left")) * maxvel,
        (Input::get_singleton()->is_action_pressed("ui_down") - Input::get_singleton()->is_action_pressed("ui_up")) * maxvel
    );

    auto &cam_pos = m_level->m_camera_pos;
    cam_pos += d;

    if (cam_pos.x < 0) {
        cam_pos.x = 0;
    } else if (cam_pos.x > m_level->m_curmap.dimensions.x * TILE_SIZE - 240) {
        cam_pos.x = m_level->m_curmap.dimensions.x * TILE_SIZE - 240;
    }

    if (cam_pos.y < 0) {
        cam_pos.y = 0;
    } else if (cam_pos.y > m_level->m_curmap.dimensions.y * TILE_SIZE - 192) {
        cam_pos.y = m_level->m_curmap.dimensions.y * TILE_SIZE - 192;
    }

    // const int maxvel = 4;
    // auto d = Vector2(
    //     (Input::get_singleton()->is_action_pressed("ui_right") - Input::get_singleton()->is_action_pressed("ui_left")) * maxvel,
    //     (Input::get_singleton()->is_action_pressed("ui_down") - Input::get_singleton()->is_action_pressed("ui_up")) * maxvel
    // );

    // /* update player position in level */
    // m_pos += d;

    // const auto cam_bounds = m_level->m_camera_bounds;
    // auto &cam_pos = m_level->m_camera_pos;
    // auto &cam_vel = m_level->m_camera_vel;

    // /* update camera position in level */
    // cam_pos.x += d.x;
    // cam_pos.y += d.y;

    // /* used by tiles to adjust their positions */
    // cam_vel.x = d.x;
    // cam_vel.y = d.y;

    // // d.x < 0
    // if (cam_pos.x < cam_bounds.get_position().x) {
    //     cam_pos.x = cam_bounds.get_position().x;

    // // d.x > 0
    // } else if (cam_pos.x > cam_bounds.get_end().x) {
    //     cam_pos.x = cam_bounds.get_end().x;
    // }

    // // d.y < 0
    // if (cam_pos.y < cam_bounds.get_position().y) {
    //     cam_pos.y = cam_bounds.get_position().y;

    // // d.y > 0
    // } else if (cam_pos.y > cam_bounds.get_end().y) {
    //     cam_pos.y = cam_bounds.get_end().y;
    // }

    // /* update where player is located on screen */
    // set_position(m_pos - cam_pos + m_level->m_camera_bounds.get_position());
}

void Player::set_pos(const Vector2 pos) {
    m_pos = pos;
}

Vector2 Player::get_pos() const {
    return m_pos;
}