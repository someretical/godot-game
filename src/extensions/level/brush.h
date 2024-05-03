#ifndef BRUSH_H
#define BRUSH_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Level;

class Brush : public Sprite2D {
	GDCLASS(Brush, Sprite2D)

protected:
	static void _bind_methods();

public:
	Brush();
	Brush(Level *root);
	~Brush();

    void _unhandled_input(const Ref<InputEvent> &event);
    void _ready() override;
	void _process(double delta) override;
    void _physics_process(double delta) override;

    Vector2i get_grid_pos(const Vector2 pos) const;

    bool handle_next_tile_group(const Ref<InputEvent> &event);
    bool handle_next_tile_variant(const Ref<InputEvent> &event);
    void set_tile(const int tile);
    int get_tile() const;
    void set_variant(const int variant);
    int get_variant() const;

    bool handle_pick_tile(const Ref<InputEvent> &event);

	Level *m_level;
    int m_tile_group;
    int m_variant;
};

}

#endif