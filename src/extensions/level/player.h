#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class Level;

class Player : public Sprite2D {
	GDCLASS(Player, Sprite2D)

protected:
	static void _bind_methods();

public:
	Player();
	Player(Level *level, Vector2 pos);
	~Player();

	void _process(double delta) override;
	void _physics_process(double delta) override;
	bool Player::check_collision(Vector2 pos);
	void Player::process_dx(float dx);
	void Player::process_dy(float dy);

	void set_pos(const Vector2 pos);
	Vector2 get_pos() const;
	void set_vel(const Vector2 vel);
	Vector2 get_vel() const;

	Level *m_level;
	/* Keeps track of where the player actually is on the map */
	/* Different than the coordinates of the player on the screen! */
	Vector2 m_pos;
	Vector2 m_vel;
};

}

#endif