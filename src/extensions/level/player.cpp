#include "level.h"
#include "player.h"
#include "skidcloud.h"

#include <cassert>
#include <algorithm>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

using namespace godot;

constexpr float GRAVITY_ACCEL = 0.6;
constexpr float MAX_FALL_SPEED = 10;
constexpr int JUMP_DELAY = 4;
constexpr float JUMP_SPEED = -6.1;
constexpr int MAX_JUMP_TIME = 24;
constexpr int CONTINUOUS_JUMP_DELAY = 4;
constexpr int DIRECTION_RIGHT = 1;
constexpr int DIRECTION_LEFT = -1;
constexpr float MAX_SPEED_X = 3.3;
constexpr float X_ACCEL = 0.1;
constexpr float X_DECEL = 0.95;
constexpr float X_CHANGE_DIR_ACCEL = 0.2;
constexpr int SMOKE_PARTICLE_DELAY = 7;

/*
Player overview:

+---+---+----+---+---+
|         11         |
+   +---+----+---+   +
|   |     21     |   |
+   +   +----+   +   +
| 8 | 8 | 00 | 8 | 8 |
+   +   +----+   +   +
|   |     25     |   |
+   +---+----+---+   +
|         7          |
+---+---+----+---+---+

- innermost box represents the centre of the sprite walk1
- middle box represents the player hitbox (offset)
- outer box represents the padding to ensure the hitbox fits within a 32x64 space (gap)
*/

constexpr int HITBOX_HEIGHT = 46;
constexpr int HITBOX_WIDTH = 16;
constexpr int HITBOX_TOP_OFFSET = 21;
constexpr int HITBOX_TOP_GAP = 11;
constexpr int HITBOX_BOTTOM_OFFSET = 25;
constexpr int HITBOX_BOTTOM_GAP = 7;
constexpr int HITBOX_LEFT_OFFSET = 8;
constexpr int HITBOX_LEFT_GAP = 8;
constexpr int HITBOX_RIGHT_OFFSET = 8;
constexpr int HITBOX_RIGHT_GAP = 8;

static_assert(HITBOX_RIGHT_GAP + HITBOX_RIGHT_OFFSET == HALF_TILE);
static_assert(HITBOX_LEFT_GAP + HITBOX_LEFT_OFFSET == HALF_TILE);
static_assert(HITBOX_TOP_GAP + HITBOX_TOP_OFFSET == TILE_SIZE);
static_assert(HITBOX_BOTTOM_GAP + HITBOX_BOTTOM_OFFSET == TILE_SIZE);
static_assert(HITBOX_TOP_OFFSET + HITBOX_BOTTOM_OFFSET == HITBOX_HEIGHT);
static_assert(HITBOX_LEFT_OFFSET + HITBOX_RIGHT_OFFSET == HITBOX_WIDTH);
static_assert(HITBOX_TOP_GAP + HITBOX_TOP_OFFSET + HITBOX_BOTTOM_GAP + HITBOX_BOTTOM_OFFSET == TILE_SIZE * 2);
static_assert(HITBOX_LEFT_GAP + HITBOX_LEFT_OFFSET + HITBOX_RIGHT_GAP + HITBOX_RIGHT_OFFSET == TILE_SIZE);

const char *PLAYER_ANIMATIONS[] = {
	"idle",
	"walk",
	"turn",
	"jump",
	"fall"
};

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_pos"), &Player::get_pos);
    ClassDB::bind_method(D_METHOD("set_pos", "pos"), &Player::set_pos);
    ClassDB::add_property("Player", PropertyInfo(Variant::VECTOR2, "m_pos"), "set_pos", "get_pos");

    ClassDB::bind_method(D_METHOD("get_vel"), &Player::get_vel);
    ClassDB::bind_method(D_METHOD("set_vel", "vel"), &Player::set_vel);
    ClassDB::add_property("Player", PropertyInfo(Variant::VECTOR2, "m_vel"), "set_vel", "get_vel");

    ClassDB::bind_method(D_METHOD("get_fall_time"), &Player::get_fall_time);
    ClassDB::bind_method(D_METHOD("set_fall_time", "m_fall_time"), &Player::set_fall_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::INT, "m_fall_time"), "set_fall_time", "get_fall_time");

    ClassDB::bind_method(D_METHOD("get_jump_time"), &Player::get_jump_time);
    ClassDB::bind_method(D_METHOD("set_jump_time", "m_jump_time"), &Player::set_jump_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::INT, "m_jump_time"), "set_jump_time", "get_jump_time");

    ClassDB::bind_method(D_METHOD("get_ground_time"), &Player::get_ground_time);
    ClassDB::bind_method(D_METHOD("set_ground_time", "m_ground_time"), &Player::set_ground_time);
    ClassDB::add_property("Player", PropertyInfo(Variant::INT, "m_ground_time"), "set_ground_time", "get_ground_time");

    ClassDB::bind_method(D_METHOD("get_direction"), &Player::get_direction);
    ClassDB::bind_method(D_METHOD("set_direction", "m_direction"), &Player::set_direction);
    ClassDB::add_property("Player", PropertyInfo(Variant::INT, "m_direction"), "set_direction", "get_direction");

    ClassDB::bind_method(D_METHOD("get_jump_start_x"), &Player::get_jump_start_x);
    ClassDB::bind_method(D_METHOD("set_jump_start_x", "m_jump_start_x"), &Player::set_jump_start_x);
    ClassDB::add_property("Player", PropertyInfo(Variant::FLOAT, "m_jump_start_x"), "set_jump_start_x", "get_jump_start_x");
}

Player::Player() {
}

Player::Player(Level *level, Vector2 pos) : m_level(level), m_pos(pos) {
    m_jump_time = 0;
    m_fall_time = 0;
    m_ground_time = 0;
    m_direction = DIRECTION_RIGHT;
    m_jump_start_x = 0;
    m_action = PlayerAction::IDLE;

    set_process_priority(static_cast<int>(ProcessingPriority::Player));
    set_physics_process_priority(static_cast<int>(PhysicsProcessingPriority::Player));
    set_z_index(static_cast<int>(ZIndex::Player));

    set_sprite_frames(ResourceLoader::get_singleton()->load("src/assets/player/animation.tres"));
    set_animation(PLAYER_ANIMATIONS[static_cast<int>(m_action)]);
    set_flip_h(false);
}

Player::~Player() {
}

void Player::_process(double delta) {
    if (m_fall_time > 0) {
        if (m_vel.y < 0) {
            set_animation("jump");
        } else {
            /* set texture to the falling one */
            set_animation("fall");
        }
    } else {
        /* set texture to walking one */
        if (m_vel.x == 0) {
            if (get_animation() == StringName("walk")) {
                if (get_frame() == 0) {
                    set_animation("idle");
                } else {
                    /* do nothing as we want a smooth transition to the idle frame */
                }
            } else {
                set_animation("idle");
            }
        } else {
            if (m_action == PlayerAction::TURNING) {
                set_animation(PLAYER_ANIMATIONS[static_cast<int>(m_action)]);
            } else if (m_action != PlayerAction::WALKING || (get_animation() != StringName("walk") && m_action == PlayerAction::WALKING)) {
                m_action = PlayerAction::WALKING;
                set_animation(PLAYER_ANIMATIONS[static_cast<int>(m_action)]);
                play();
            }
            set_speed_scale(std::min(std::max(0.6f, abs(m_vel.x) * 0.6f), 2.0f));
        }
    }

    /* the sprites should be facing right by default */
    if (m_direction == DIRECTION_RIGHT) {
        set_flip_h(false);
    } else if (m_direction == DIRECTION_LEFT) {
        set_flip_h(true);
    }
}

bool Player::check_collision(Vector2 pos) const {
    /* actual player spite size is 28x54 */
    /* the player has a hitbox size of 16x50 about m_pos */
    /* except the top half of the hitbox is 4px shorter*/
    /* pos is centred in the player hitbox sprite which is 16x32 */
    const auto &player_hitbox = Rect2(pos.x - HITBOX_LEFT_OFFSET, pos.y - HITBOX_TOP_OFFSET, HITBOX_WIDTH, HITBOX_HEIGHT);

    /* check that player is still within level */
    if (!m_level->m_bounds.encloses(player_hitbox)) {
        return true;
    }

    /* get top left tile indices */
    const auto topleft = Vector2i(player_hitbox.get_position().x / TILE_SIZE, player_hitbox.get_position().y / TILE_SIZE);
    const auto bottomright = Vector2i(player_hitbox.get_end().x / TILE_SIZE, player_hitbox.get_end().y / TILE_SIZE);

    /* check if any part of the player intersects with tiles */
    for (int i = topleft.x; i <= bottomright.x; i++) {
        for (int j = topleft.y; j <= bottomright.y; j++) {
            /* assume that all blocks have a hitbox that is 16x16 */
            if (m_level->m_curmap.tile_data[j][i].m_tile_group != -1) {
                const auto tile_hitbox = Rect2i(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE);

                if (player_hitbox.intersects(tile_hitbox)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Player::process_x() {
    bool left_pressed = Input::get_singleton()->is_action_pressed("ui_left");
    bool right_pressed = Input::get_singleton()->is_action_pressed("ui_right");

    m_direction = right_pressed - left_pressed;

    /*
    in most games, it's fine to handle acceleration and deceleration the same way
    however, in mario games, the player accelerates slower than usual
    also if the player tries to turn around while moving, they will decelerate faster
    */
    if (m_direction == 0) {
        /* handle deceleration */
        /* in mario games, the player only decelerates if they are in contact with the ground */
        if (m_fall_time < JUMP_DELAY) {
            m_vel.x = m_vel.x * X_DECEL;
            if (abs(m_vel.x) < 0.1) {
                m_vel.x = 0;
            }
        }
    } else {
        /* handle acceleration */
        if (m_direction * m_vel.x < MAX_SPEED_X - X_ACCEL) {

            if (m_direction * m_vel.x < 0) {
                /* if we are turning around then the deceleration should be greater */
                m_vel.x += m_direction * X_CHANGE_DIR_ACCEL;

                m_action = PlayerAction::TURNING;

                /* create skid cloud every SMOKE_PARTICLE_DELAY ticks the player is on the ground */
                if (m_fall_time == 0 && m_ground_time % SMOKE_PARTICLE_DELAY == 0) {
                    SkidCloud *cloud = memnew(SkidCloud(m_level, m_pos));
                    m_level->m_particles_node->add_child(cloud);
                }
            } else {
                m_action = PlayerAction::WALKING;
                m_vel.x += m_direction * X_ACCEL;
            }
        }
    }

    const auto new_pos = m_pos + Vector2(m_vel.x, 0);
    if (check_collision(new_pos)) {
        if (m_vel.x > 0) {
            /*
            In these roundf functions (and the ones in process_y), the calculations assume that:
            - as of m_pos, the centre of the player is not within any other tile hitbox
            - as of new_pos, the centre of the player is STILL not within any other tile hitbox
            
            HOWEVER, as of new_pos, the boundary of the player's hitbox MAY be within a tile hitbox which triggers the collision check
            as such, if m_vel.x is greater than the horizontal distance between the player's centre and the edge of the player's hitbox, the player will instead clip INTO the tile it just collided with
            the only way to fix this is to add an extra check but since collision checking is expensive, the cheap alternative is to just mention the constraint that the maximum velocity in a direction should be less than or equal to the minimum distance between the player's centre and the edge of the player's hitbox
            this affects both X and Y directions but extra care should be taken in the Y direction as the hitbox is usually not symmetrical
            */
            m_pos.x = roundf(new_pos.x + (HALF_TILE + HITBOX_RIGHT_GAP) - fmodf(new_pos.x, TILE_SIZE));
        } else {
            m_pos.x = roundf(new_pos.x + (HALF_TILE - HITBOX_LEFT_GAP) - fmodf(new_pos.x, TILE_SIZE));
        }

        /* make the player fall straight down if they were in the middle of a jump and ran into something horizontally */
        /* we store m_jump_start_x to ensure that if the player was already against a wall, they can jump to full height and move as necessary */
        if (m_pos.x != m_jump_start_x && m_jump_time > 0) {
            m_jump_time = MAX_JUMP_TIME + 1;
            m_jump_start_x = 0;
        }

        m_vel.x = 0;
    } else {
        m_pos.x += m_vel.x;
    }
}

void Player::process_y() {
    m_fall_time++;

    /* simulate gravity */
    m_vel.y += GRAVITY_ACCEL;
    if (m_vel.y > MAX_FALL_SPEED) {
        m_vel.y = MAX_FALL_SPEED;
    }

    bool up_pressed = Input::get_singleton()->is_action_pressed("ui_up");
    bool down_pressed = Input::get_singleton()->is_action_pressed("ui_down");

    /*
    Jumping notes:
    - can only begin a jump if the player has recently touched the ground (JUMP_DELAY)
    - this has the added effect of allowing a player to jump if they have recently started falling off a ledge for example
        - this is useful if we ever have any sloped surfaces (probably not...)

    - JUMPING_TIME keeps track of how long the player has been pressing the jump button
    - MAX_JUMP_TIME is the maximum amount of time the player can press the jump button to jump
    - CONTINUOUS_JUMP_DELAY is the amount of time the player has to wait before they can jump again after they have touched the ground
        - this is useful for hollow knight style jumping
    */
    bool jumping = up_pressed;
    if (jumping) {
        if (m_fall_time < JUMP_DELAY || m_jump_time > 0) {
            if (m_jump_time == 0) {
                m_jump_start_x = m_pos.x;
            }

            m_jump_time++;

            if (m_jump_time < MAX_JUMP_TIME) {
                m_ground_time = 0;
                m_vel.y = JUMP_SPEED;
            }
        }
    } else {
        m_jump_time = 0;
    }

    const auto new_pos = m_pos + Vector2(0, m_vel.y);
    if (check_collision(new_pos)) {
        if (m_vel.y > 0) {
            /* player has fallen down and is now resting on a tile */
            m_fall_time = 0;

            /* 
            following code controls continuous jumping behaviour 
            if commented, player will not be able to hold down the jump button to jump straight after they have touched the ground again
            this behaviour is present in games like mario

            if uncommented, then we have hollow knight style jumping
            */
            m_ground_time++;
            if (m_ground_time > CONTINUOUS_JUMP_DELAY) {
                m_jump_time = 0;
            }

            m_pos.y = roundf(new_pos.y + HITBOX_BOTTOM_GAP - fmodf(new_pos.y, TILE_SIZE));
        } else {
            /* player has hit their head on the bottom side of a tile */
            /* increase jump time artificially so the player cannot "stick" to the bottom side of the side */
            m_jump_time = MAX_JUMP_TIME + 1;

            /* the 11 is the empty number of pixels between the top of the hitbox and the edge of the sprite */
            m_pos.y = roundf(new_pos.y + (TILE_SIZE - HITBOX_TOP_GAP) - fmodf(new_pos.y, TILE_SIZE));
        }
        m_vel.y = 0;
    } else {
        m_pos.y += m_vel.y;
    }
}

void Player::_physics_process(double delta) {
    /* there is a bug where if the player's velocity is too high, they clip into blocks */
    /* I don't know the root cause of it... */
    process_y();
    process_x();

    /* update camera location in level */
    m_level->update_camera();

    /* update where player is located on screen */
    set_position(m_pos - m_level->m_camera_pos);
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

void Player::set_fall_time(const int fall_time) {
    m_fall_time = fall_time;
}

int Player::get_fall_time() const {
    return m_fall_time;
}

void Player::set_jump_time(const int jump_time) {
    m_jump_time = jump_time;
}

int Player::get_jump_time() const {
    return m_jump_time;
}

void Player::set_ground_time(const int ground_time) {
    m_ground_time = ground_time;
}

int Player::get_ground_time() const {
    return m_ground_time;
}

void Player::set_direction(const int direction) {
    m_direction = direction;
}

int Player::get_direction() const {
    return m_direction;
}

void Player::set_jump_start_x(const float jump_start_x) {
    m_jump_start_x = jump_start_x;
}

float Player::get_jump_start_x() const {
    return m_jump_start_x;
}
