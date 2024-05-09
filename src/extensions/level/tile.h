#ifndef TILE_H
#define TILE_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Level;

class Tile : public Sprite2D {
	GDCLASS(Tile, Sprite2D)

protected:
	static void _bind_methods();

public:
	Tile();
	Tile(Level *root, Vector2i tile_index);
	~Tile();

	void _process(double delta) override;
	void _physics_process(double delta) override;

	Vector2 m_pos;
	Vector2i m_grid_indices;
	Level *m_level;
};

}

#endif