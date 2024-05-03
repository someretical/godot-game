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

    void handle_next_tile_group(const Ref<InputEvent> &event);
    void handle_next_tile_variant(const Ref<InputEvent> &event);
    void set_tile(const int tile);
    int get_tile() const;
    void set_variant(const int variant);
    int get_variant() const;

    void handle_place_tile(const Ref<InputEvent> &event);

	Level *m_level;
    int m_tile;
    int m_variant;
};

}

#endif