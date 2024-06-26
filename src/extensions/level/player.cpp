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

static_assert(HITBOX_RIGHT_GAP + HITBOX_RIGHT_OFFSET == HALF_TILE);
static_assert(HITBOX_LEFT_GAP + HITBOX_LEFT_OFFSET == HALF_TILE);
static_assert(HITBOX_TOP_GAP + HITBOX_TOP_OFFSET == TILE_SIZE);
static_assert(HITBOX_BOTTOM_GAP + HITBOX_BOTTOM_OFFSET == TILE_SIZE);
static_assert(HITBOX_TOP_OFFSET + HITBOX_BOTTOM_OFFSET == HITBOX_HEIGHT);
static_assert(HITBOX_LEFT_OFFSET + HITBOX_RIGHT_OFFSET == HITBOX_WIDTH);
static_assert(HITBOX_TOP_GAP + HITBOX_TOP_OFFSET + HITBOX_BOTTOM_GAP + HITBOX_BOTTOM_OFFSET == TILE_SIZE * 2);
static_assert(HITBOX_LEFT_GAP + HITBOX_LEFT_OFFSET + HITBOX_RIGHT_GAP + HITBOX_RIGHT_OFFSET == TILE_SIZE);

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
constexpr float MAX_GOD_SPEED = 8;
constexpr int SMOKE_PARTICLE_DELAY = 7;

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
    ClassDB::add_property("Player", PropertyInfo(Variant::VECTOR2, "m_true_pos"), "set_pos", "get_pos");

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

Player::Player(Level *level, Vector2 pos) : m_level(level), m_true_pos(pos) {
    m_jump_time = 0;
    m_fall_time = 0;
    m_ground_time = 0;
    m_direction = DIRECTION_RIGHT;
    m_jump_start_x = 0;
    m_action = PlayerAction::IDLE;

    set_name("Player 2DAnimatedSprite");

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

bool Player::check_collision(Vector2 pos) {
    /* see hitbox notes at the top of this file */
    const auto &player_hitbox = Rect2(pos.x - HITBOX_LEFT_OFFSET, pos.y - HITBOX_TOP_OFFSET, HITBOX_WIDTH, HITBOX_HEIGHT);

    /* check that player is still within level */
    /*
    this is one place where we need to use the tiny constant:
    if the player hitbox is perfectly aligned with the bottom edge of the level, the following check will return true
    however, when we compute bottomright, as the bottom edge of the player's hitbox is the same as the bottom bound of the level, it is therefore divisible by TILE_SIZE because the level size is divisible by TILE_SIZE
    this means there is no fractional part to truncate and therefore we end up with an array index that is out of bounds
    you can test this out by setting tiny to 0f and then running the game and using the level editor to remove the ground and letting the player fall
    the debugger will highlight the if condition inside the double for loop when we check the tile group and you will see that j is out of bounds
    so by subtracting and adding TINY in certain places such as when computing the level bounds, we can ensure that checks dependent on truncation continue to work as expected
    */
    if (!m_level->m_bounds.encloses(player_hitbox)) {
        return true;
    }

    /* get top left tile indices */
    const auto topleft = Vector2i(player_hitbox.get_position().x / TILE_SIZE, player_hitbox.get_position().y / TILE_SIZE);
    const auto bottomright = Vector2i(player_hitbox.get_end().x / TILE_SIZE, player_hitbox.get_end().y / TILE_SIZE);

    /* check collisions at head */
    for (int i = topleft.x; i <= bottomright.x; i++) {
        HitboxPartBitset parts{};
        parts.set(HITBOX_HEAD);
        if (check_collision_at_hitbox_part(Vector2i(i, topleft.y), player_hitbox, parts)) {
            return true;
        }
    }

    /* check collisions in body */
    for (int i = topleft.x; i <= bottomright.x; i++) {
        for (int j = topleft.y + 1; j <= bottomright.y - 1; j++) {
            HitboxPartBitset parts{};
            parts.set(HITBOX_BODY);
            if (check_collision_at_hitbox_part(Vector2i(i, j), player_hitbox, parts)) {
                return true;
            }
        }
    }

    /* check collisions at feet */
    for (int i = topleft.x; i <= bottomright.x; i++) {
        HitboxPartBitset parts{};
        parts.set(HITBOX_FEET);
        if (check_collision_at_hitbox_part(Vector2i(i, bottomright.y), player_hitbox, parts)) {
            return true;
        }
    }

    return false;
}

bool Player::check_collision_at_hitbox_part(const Vector2i tile_grid_index, const Rect2 player_hitbox, const HitboxPartBitset parts) {
    auto &tile = m_level->m_curmap->m_tile_data[tile_grid_index.y][tile_grid_index.x];

    if (parts.test(HITBOX_HEAD)) {
        switch (tile.m_tile_group) {
            /* blue tiles */
            case 0: {
ignore_blue:
                /* only check collisions with the top variant of blue tiles with the feet */
                return false;
            }
        }
    }

    if (parts.test(HITBOX_BODY)) {
        switch (tile.m_tile_group) {
            case 0: goto ignore_blue;
        }
    }

    if (parts.test(HITBOX_FEET)) {
        switch (tile.m_tile_group) {
            case 0: {
                if (tile.m_variant >= 0 && tile.m_variant < 6) {
                    return false;
                }
                
                else {
                    /* if the player is holding down, we want them to go through platforms */
                    if (Input::get_singleton()->is_action_pressed("ui_down")) return false;

                    /* if the player's feet is falling and is passing through the top of a platform */
                    /* note that downwards is positive */
                    if (TILE_SIZE - fmodf(player_hitbox.get_end().y, TILE_SIZE) + m_vel.y <= TILE_SIZE) return false;

                    return true;
                }
            }
        }
    }

    switch (tile.m_tile_group) {
        /* empty */
        case -1: return false;

        /* coin */
        case 4: {
            const Rect2i tile_hitbox{tile_grid_index.x * TILE_SIZE, tile_grid_index.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (player_hitbox.intersects(tile_hitbox)) {
                /* TODO store number of collected coins */
                tile.m_tile_group = -1;
                return false;
            }
        }

        default: {
            const Rect2i tile_hitbox{tile_grid_index.x * TILE_SIZE, tile_grid_index.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            return player_hitbox.intersects(tile_hitbox);
        }
    }
}

void Player::process_x() {
    bool left_pressed = Input::get_singleton()->is_action_pressed("ui_left");
    bool right_pressed = Input::get_singleton()->is_action_pressed("ui_right");

    m_direction = right_pressed - left_pressed;

    if (m_level->m_editor.m_enabled) {
        m_true_pos.x += m_direction * MAX_GOD_SPEED;
        return;
    }

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
                    SkidCloud *cloud = memnew(SkidCloud(m_level, m_true_pos));
                    m_level->m_particles_node->add_child(cloud);
                }
            } else {
                m_action = PlayerAction::WALKING;
                m_vel.x += m_direction * X_ACCEL;
            }
        }
    }

    const auto new_pos = m_true_pos + Vector2(m_vel.x, 0);
    if (check_collision(new_pos)) {
        if (m_vel.x > 0) {
            /*
            In these roundf functions (and the ones in process_y), the calculations assume that:
            - as of m_true_pos, the centre of the player is not within any other tile hitbox
            - as of new_pos, the centre of the player is STILL not within any other tile hitbox
            
            HOWEVER, as of new_pos, the boundary of the player's hitbox MAY be within a tile hitbox which triggers the collision check
            as such, if m_vel.x is greater than the horizontal distance between the player's centre and the edge of the player's hitbox, the player will instead clip INTO the tile it just collided with
            the only way to fix this is to add an extra check but since collision checking is expensive, the cheap alternative is to just mention the constraint that the maximum velocity in a direction should be less than or equal to the minimum distance between the player's centre and the edge of the player's hitbox
            this affects both X and Y directions but extra care should be taken in the Y direction as the hitbox is usually not symmetrical
            */
            m_true_pos.x = roundf(new_pos.x + (HALF_TILE + HITBOX_RIGHT_GAP) - fmodf(new_pos.x, TILE_SIZE)) - (TINY + TINIER);
        } else {
            m_true_pos.x = roundf(new_pos.x + (HALF_TILE - HITBOX_LEFT_GAP) - fmodf(new_pos.x, TILE_SIZE)) + (TINY + TINIER);
        }

        /* make the player fall straight down if they were in the middle of a jump and ran into something horizontally */
        /* we store m_jump_start_x to ensure that if the player was already against a wall, they can jump to full height and move as necessary */
        if (m_true_pos.x != m_jump_start_x && m_jump_time > 0) {
            m_jump_time = MAX_JUMP_TIME + 1;
            m_jump_start_x = 0;
        }

        m_vel.x = 0;
    } else {
        m_true_pos.x += m_vel.x;
    }
}

void Player::process_y() {
    bool up_pressed = Input::get_singleton()->is_action_pressed("ui_up");
    bool down_pressed = Input::get_singleton()->is_action_pressed("ui_down");
    
    int direction = down_pressed - up_pressed;
    if (m_level->m_editor.m_enabled) {
        m_true_pos.y += direction * MAX_GOD_SPEED;
        return;
    }

    m_fall_time++;

    /* simulate gravity */
    m_vel.y += GRAVITY_ACCEL;
    if (m_vel.y > MAX_FALL_SPEED) {
        m_vel.y = MAX_FALL_SPEED;
    }

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
                m_jump_start_x = m_true_pos.x;
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

    const auto new_pos = m_true_pos + Vector2(0, m_vel.y);
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

            m_true_pos.y = roundf(new_pos.y + HITBOX_BOTTOM_GAP - fmodf(new_pos.y, TILE_SIZE)) - (TINY + TINIER);
        } else {
            /* player has hit their head on the bottom side of a tile */
            /* increase jump time artificially so the player cannot "stick" to the bottom side of the side */
            m_jump_time = MAX_JUMP_TIME + 1;

            /* the 11 is the empty number of pixels between the top of the hitbox and the edge of the sprite */
            m_true_pos.y = roundf(new_pos.y + (TILE_SIZE - HITBOX_TOP_GAP) - fmodf(new_pos.y, TILE_SIZE)) + (TINY + TINIER);
        }
        m_vel.y = 0;
    } else {
        m_true_pos.y += m_vel.y;
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
    set_position(m_true_pos - m_level->m_camera_true_pos);
}

void Player::set_pos(const Vector2 pos) {
    m_true_pos = pos;
}

Vector2 Player::get_pos() const {
    return m_true_pos;
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
