#include "console.h"
#include "level.h"

#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Console::_bind_methods() {
}

Console::Console() {
}

Console::Console(Level *level) : m_level(level) {
    set_name("Console control node");
    set_size(Vector2{CAMERA_WIDTH, CAMERA_HEIGHT});
    set_position(Vector2{-CAMERA_WIDTH/2, -CAMERA_HEIGHT/2});

    auto panel_container = memnew(PanelContainer);
    if (!panel_container) {
        std::exit(1);
    }
    panel_container->set_name("Panel container");
    panel_container->set_size(Vector2{CAMERA_WIDTH, 140});
    add_child(panel_container);

    auto margin_container1 = memnew(MarginContainer);
    if (!margin_container1) {
        std::exit(1);
    }
    margin_container1->set_name("Panel margin container");
    margin_container1->add_theme_constant_override("margin_left", 5);
    margin_container1->add_theme_constant_override("margin_right", 5);
    margin_container1->add_theme_constant_override("margin_top", 5);
    margin_container1->add_theme_constant_override("margin_bottom", 5);
    panel_container->add_child(margin_container1);

    auto v_box_container = memnew(VBoxContainer);
    if (!v_box_container) {
        std::exit(1);
    }
    v_box_container->set_name("Panel v box container");
    margin_container1->add_child(v_box_container);

    auto margin_container2 = memnew(MarginContainer);
    if (!margin_container2) {
        std::exit(1);
    }
    margin_container2->set_name("Rich text margin container");
    margin_container2->add_theme_constant_override("margin_bottom", 5);
    margin_container2->set_v_size_flags(Control::SizeFlags::SIZE_EXPAND_FILL);
    v_box_container->add_child(margin_container2);

    m_rich_text_label = memnew(RichTextLabel);
    if (!m_rich_text_label) {
        std::exit(1);
    }
    m_rich_text_label->set_name("Console output rich text label");
    margin_container2->add_child(m_rich_text_label);

    m_line_edit = memnew(LineEdit);
    if (!m_line_edit) {
        std::exit(1);
    }
    m_line_edit->set_name("Console prompt line edit");
    m_line_edit->set_placeholder("Enter command...");
    v_box_container->add_child(m_line_edit);

    set_visible(true);
}

Console::~Console() {
}

void Console::_ready() {
}

void Console::_process(double delta) {
}

void Console::_gui_input(const Ref<InputEvent> &event) {

}
