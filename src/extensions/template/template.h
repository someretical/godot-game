#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Template : public Node2D {
	GDCLASS(Template, Node2D)

protected:
	static void _bind_methods();

public:
	Template();
	~Template();

	void _process(double delta) override;
};

}

#endif