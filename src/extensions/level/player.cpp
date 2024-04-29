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
    set_texture(m_level->m_player_preloader->get_resource("player_hitbox"));
}

Player::~Player() {
}

void Player::_process(double delta) {
}

bool Player::check_collision(Vector2 pos) {
    /* assume that the player has a hitbox size of 10x20 aligned with the bottom middle */
    /* pos is centred in the player hitbox sprite which is 16x32 */
    const auto &player_hitbox = Rect2(pos.x - 5, pos.y - 4, 10, 20);

    /* get top left tile indices */
    const auto topleft = Vector2i(player_hitbox.get_position().x / TILE_SIZE, player_hitbox.get_position().y / TILE_SIZE);
    const auto bottomright = Vector2i((player_hitbox.get_end().x) / TILE_SIZE, player_hitbox.get_end().y / TILE_SIZE);

    /* check if any part of the player intersects with tiles */
    for (int i = topleft.x; i <= bottomright.x; i++) {
        for (int j = topleft.y; j <= bottomright.y; j++) {
            /* assume that all blocks have a hitbox that is 16x16 */
            if (m_level->m_curmap.tile_data[j][i] != -1) {
                const auto tile_hitbox = Rect2i(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE);

                if (player_hitbox.intersects(tile_hitbox)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Player::process_dx(float dx) {
    m_vel.x = m_vel.x * 0.8 + dx;

    const auto new_pos = m_pos + Vector2(m_vel.x, 0);
    if (check_collision(new_pos)) {
        
        if (m_vel.x > 0) {
            m_pos.x = roundf(new_pos.x + (HALF_TILE + 3) - fmodf(new_pos.x, TILE_SIZE));
        } else {
            /* the 3 is the empty number of pixels between the hitbox and the side edges of the sprite */
            m_pos.x = roundf(new_pos.x + (HALF_TILE - 3) - fmodf(new_pos.x, TILE_SIZE));
        }

        m_vel.x = 0;
    } else {
        m_pos.x += m_vel.x;
    }
}

void Player::process_dy(float dy) {
    if (dy != 0) {
        m_vel.y = dy;
    }

    m_vel.y = m_vel.y + 0.5;
    if (m_vel.y > 5) {
        m_vel.y = 5;
    }

    const auto new_pos = m_pos + Vector2(0, m_vel.y);
    if (check_collision(new_pos)) {
        if (m_vel.y > 0) {
            m_pos.y = roundf(new_pos.y - fmodf(new_pos.y, TILE_SIZE));
        } else {
            /* the 4 is the empty number of pixels between the top of the hitbox and the edge of the sprite */
            m_pos.y = roundf(new_pos.y + (HALF_TILE - 4) - fmodf(new_pos.y, TILE_SIZE));
        }
        m_vel.y = 0;
    } else {
        m_pos.y += m_vel.y;
    }
}

void Player::_physics_process(double delta) {
    const auto maxvel = Vector2{0.5, 4};
    process_dx((Input::get_singleton()->is_action_pressed("ui_right") - Input::get_singleton()->is_action_pressed("ui_left")) * maxvel.x);
    process_dy((0 - Input::get_singleton()->is_action_pressed("ui_up")) * maxvel.y);

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