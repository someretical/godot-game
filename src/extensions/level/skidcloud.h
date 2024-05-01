#ifndef SKIDCLOUD_H
#define SKIDCLOUD_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Level;

class SkidCloud : public Sprite2D {
	GDCLASS(SkidCloud, Sprite2D)

protected:
	static void _bind_methods();

public:
	SkidCloud();
	SkidCloud(Level *level, Vector2 pos);
	~SkidCloud();

	void _process(double delta) override;

	void set_pos(const Vector2 pos);
	Vector2 get_pos() const;
    void set_frame(int frame);
    int get_frame() const;

	Level *m_level;
    Vector2 m_pos;
    int m_frame;
};

}

#endif