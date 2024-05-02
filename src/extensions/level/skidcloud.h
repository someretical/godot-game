#ifndef SKIDCLOUD_H
#define SKIDCLOUD_H

#include <godot_cpp/classes/animated_sprite2d.hpp>

namespace godot {

class Level;

class SkidCloud : public AnimatedSprite2D {
	GDCLASS(SkidCloud, AnimatedSprite2D)

protected:
	static void _bind_methods();

public:
	SkidCloud();
	SkidCloud(Level *level, Vector2 pos);
	~SkidCloud();

	void _ready() override;
	void _process(double delta) override;

	void set_pos(const Vector2 pos);
	Vector2 get_pos() const;

	Level *m_level;
    Vector2 m_pos;
};

}

#endif