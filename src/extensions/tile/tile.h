#ifndef TILE_H
#define TILE_H

#include "../root/root.h"

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Tile : public Sprite2D {
	GDCLASS(Tile, Sprite2D)

protected:
	static void _bind_methods();

public:
	Tile();
	Tile(Root *root, Vector2i tile_index);
	~Tile();

	void _process(double delta) override;
	void _physics_process(double delta) override;

	Vector2i m_tile_index;
	Root *m_root;
};

}

#endif