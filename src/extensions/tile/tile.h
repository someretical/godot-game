#ifndef TILE_H
#define TILE_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Tile : public Sprite2D {
	GDCLASS(Tile, Sprite2D)

protected:
	static void _bind_methods();

public:
	Tile();
	~Tile();

	void _process(double delta) override;
};

}

#endif