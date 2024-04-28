#ifndef PLAYER_H
#define PLAYER_H

#include "level.h"

#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class Player : public Sprite2D {
	GDCLASS(Player, Sprite2D)

protected:
	static void _bind_methods();

public:
	Player();
	Player(Level *root, Vector2 pos);
	~Player();

	void _process(double delta) override;
	void _physics_process(double delta) override;

	Level *m_root;
	Vector2 m_pos;
};

}

#endif