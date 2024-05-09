#ifndef MARKER_H
#define MARKER_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Level;

class Marker : public Sprite2D {
	GDCLASS(Marker, Sprite2D)

protected:
	static void _bind_methods();

public:
	Marker();
	Marker(Level *root, Vector2i pos, const Ref<Resource> texture);
	~Marker();

	void _process(double delta) override;
	void _physics_process(double delta) override;

	Vector2i m_true_pos;
	Level *m_level;
};

}

#endif