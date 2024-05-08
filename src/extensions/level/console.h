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

    void _input(const Ref<InputEvent> &event) override;

	void handle_command(const String new_text);

	bool handle_expand_command(const PackedStringArray &args);

	Level *m_level;
    RichTextLabel *m_rich_text_label;
    LineEdit *m_line_edit;
};

}

#endif