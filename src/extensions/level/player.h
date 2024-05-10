#ifndef PLAYER_H
#define PLAYER_H

#include <bitset>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class Level;

/*
Player overview:

+---+---+----+---+---+
|         12         |
+   +---+----+---+   +
|   |     20     |   |
+   +   +----+   +   +
| 8 | 8 | 00 | 8 | 8 |
+   +   +----+   +   +
|   |     26     |   |
+   +---+----+---+   +
|         6          |
+---+---+----+---+---+

- innermost box represents the centre of the sprite walk1
- middle box represents the player hitbox (offset)
- outer box represents the padding to ensure the hitbox fits within a 32x64 space (gap)
*/

constexpr int HITBOX_HEIGHT = 46;
constexpr int HITBOX_WIDTH = 16;
constexpr int HITBOX_TOP_OFFSET = 20;
constexpr int HITBOX_TOP_GAP = 12;
constexpr int HITBOX_BOTTOM_OFFSET = 26;
constexpr int HITBOX_BOTTOM_GAP = 6;
constexpr int HITBOX_LEFT_OFFSET = 8;
constexpr int HITBOX_LEFT_GAP = 8;
constexpr int HITBOX_RIGHT_OFFSET = 8;
constexpr int HITBOX_RIGHT_GAP = 8;

enum class PlayerAction {
	IDLE,
	WALKING,
	TURNING,
	JUMPING,
	FALLING
};

enum HitboxPart {
	HITBOX_HEAD = 0,
	HITBOX_BODY = 1,
	HITBOX_FEET = 2
};

typedef std::bitset<3> HitboxPartBitset;

class Player : public AnimatedSprite2D {
	GDCLASS(Player, AnimatedSprite2D)

protected:
	static void _bind_methods();

public:
	Player();
	Player(Level *level, Vector2 pos);
	~Player();

	void _process(double delta) override;
	void _physics_process(double delta) override;
	bool check_collision_at_hitbox_part(const Vector2i tile_grid_index, const Rect2 player_hitbox, const HitboxPartBitset parts);
	bool check_collision(Vector2 pos);
	void process_x();
	void process_y();

	void set_pos(const Vector2 pos);
	Vector2 get_pos() const;
	void set_vel(const Vector2 vel);
	Vector2 get_vel() const;
	void set_fall_time(const int fall_time);
	int get_fall_time() const;
	void set_jump_time(const int jump_time);
	int get_jump_time() const;
	void set_ground_time(const int ground_time);
	int get_ground_time() const;
	void set_direction(const int direction);
	int get_direction() const;
	void set_jump_start_x(const float jump_start_x);
	float get_jump_start_x() const;

	Level *m_level;
	/* Keeps track of where the player actually is on the map */
	/* Different than the coordinates of the player on the screen! */
	Vector2 m_true_pos;
	Vector2 m_vel;
	int m_jump_time;
	int m_fall_time;
	int m_ground_time;
	int m_direction;
	float m_jump_start_x;
	enum PlayerAction m_action;
};

}

#endif