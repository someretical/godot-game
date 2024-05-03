#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class Level;

enum class PlayerAction {
	IDLE,
	WALKING,
	TURNING,
	JUMPING,
	FALLING
};

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
	bool check_collision(Vector2 pos) const;
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
	Vector2 m_pos;
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