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
    // set_size(Vector2{CAMERA_WIDTH, CAMERA_HEIGHT});
    // set_position(Vector2{-CAMERA_WIDTH/2, -CAMERA_HEIGHT/2});

    // auto panel_container = memnew(PanelContainer);
    // if (!panel_container) {
    //     std::exit(1);
    // }
    // panel_container->set_size(Vector2{CAMERA_WIDTH, 140});
    // add_child(panel_container);

    // auto margin_container1 = memnew(MarginContainer);
    // if (!margin_container1) {
    //     memfree(panel_container);
    //     std::exit(1);
    // }
    // margin_container1->set_theme





    // set_anchors_preset(Control::PRESET_CENTER_TOP);
    // set_size(Vector2(CAMERA_WIDTH, 135));
    // set_position(Vector2(-CAMERA_WIDTH / 2, -CAMERA_HEIGHT / 2));

    // m_rich_text_label = memnew(RichTextLabel);
    // if (!m_rich_text_label) {
    //     std::exit(1);
    // }
    // m_rich_text_label->set_anchors_preset(Control::PRESET_CENTER_TOP);
    // m_rich_text_label->set_size(Vector2{CAMERA_WIDTH - 10, 90});
    // m_rich_text_label->set_position(Vector2{(-CAMERA_WIDTH / 2) + 5, 5});
    // m_rich_text_label->set_text("Welcome to the console!\nType /help to get see a list of commands.");
    // add_child(m_rich_text_label);

    // m_line_edit = memnew(LineEdit);
    // if (!m_line_edit) {
    //     memfree(m_rich_text_label);
    //     std::exit(1);
    // }
    // m_line_edit->set_anchors_preset(Control::PRESET_CENTER_TOP);
    // m_line_edit->set_size(Vector2{CAMERA_WIDTH - 10, 31});
    // m_line_edit->set_position(Vector2{(-CAMERA_WIDTH / 2) + 5, 100});
    // m_line_edit->set_placeholder("Enter command...");
    // add_child(m_line_edit);
}

Console::~Console() {
}

void Console::_ready() {
}

void Console::_process(double delta) {
}

void Console::_gui_input(const Ref<InputEvent> &event) {

}
