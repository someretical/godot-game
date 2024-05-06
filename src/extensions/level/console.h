#ifndef CONSOLE_H
#define CONSOLE_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/rich_text_label.hpp>
#include <godot_cpp/classes/line_edit.hpp>

namespace godot {

class Level;

class Console : public Control {
	GDCLASS(Console, Control)

protected:
	static void _bind_methods();

public:
	Console();
	Console(Level *level);
	~Console();

	void _ready() override;
	void _process(double delta) override;
    void _gui_input(const Ref<InputEvent> &event) override;

	Level *m_level;
    RichTextLabel *m_rich_text_label;
    LineEdit *m_line_edit;
};

}

#endif