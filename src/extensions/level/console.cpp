#include "console.h"
#include "level.h"
#include "mapdata.h"
#include "player.h"

#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input_event_action.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>

using namespace godot;

void Console::_bind_methods() {
    ClassDB::bind_method(D_METHOD("handle_command"), &Console::handle_command);
}

Console::Console() {
}

Console::Console(Level *level) : m_level(level) {
    set_name("Console control node");
    set_process_mode(ProcessMode::PROCESS_MODE_WHEN_PAUSED);
    set_size(Vector2{CAMERA_WIDTH, CAMERA_HEIGHT});
    set_position(Vector2{-CAMERA_WIDTH/2, -CAMERA_HEIGHT/2});

    auto panel_container = memnew(PanelContainer);
    if (!panel_container) {
        std::exit(1);
    }
    panel_container->set_name("Panel container");
    panel_container->set_size(Vector2{CAMERA_WIDTH, CAMERA_HEIGHT});
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
    m_rich_text_label->set_text("Welcome to the console!\nType help for a list of commands\n");
    margin_container2->add_child(m_rich_text_label);

    m_line_edit = memnew(LineEdit);
    if (!m_line_edit) {
        std::exit(1);
    }
    m_line_edit->set_name("Console prompt line edit");
    m_line_edit->set_placeholder("Enter command...");
    m_line_edit->connect("text_submitted", Callable(this, "handle_command"));
    v_box_container->add_child(m_line_edit);

    set_visible(false);
}

Console::~Console() {
}

bool Console::handle_expand_command(const PackedStringArray &args) {
    if (args.size() != 3) {
        return false;
    }

    const auto direction = args[1];
    if (direction != "u" && direction != "d" && direction != "l" && direction != "r") {
        return false;
    }

    if (!args[2].is_valid_int()) {
        return false;
    }

    const int amount = args[2].to_int();
    /* the following code is unoptimized as fuck */
    if (args[1] == "d") {
        Vector2i absolute_offset{0, abs(amount)};
        Vector2i offset = ((amount > 0) - (amount < 0)) * absolute_offset;
        Vector2i cur_dimensions = m_level->m_curmap->m_dimensions;
        Vector2i new_dimensions = m_level->m_curmap->m_dimensions + offset;

        if (new_dimensions.x <= TILE_COUNT_X || new_dimensions.y <= TILE_COUNT_Y) {
            m_rich_text_label->append_text(String("Map cannot be resized smaller than ") + TILE_COUNT_X + String(" ") + TILE_COUNT_Y + String("\n"));
            return true;
        }

        if (auto map = MapData::load_bare_map(new_dimensions); map.has_value()) {
            const auto result = map.value();

            for (int j = 0; j < std::min(cur_dimensions.y, new_dimensions.y); j++) {
                for (int i = 0; i < cur_dimensions.x; i++) {
                    result->m_tile_data[j][i] = m_level->m_curmap->m_tile_data[j][i];
                }
            }

            m_level->m_curmap = result;
            m_level->m_bounds = Rect2{
                0 + TINY, 
                0 + TINY, 
                static_cast<float>(new_dimensions.x * TILE_SIZE) - (2 * TINY), 
                static_cast<float>(new_dimensions.y * TILE_SIZE) - (2 * TINY)
            };
            m_level->m_player->m_true_pos.y += amount * TILE_SIZE;

            m_rich_text_label->append_text("Map expanded\n");
        } else {
            m_rich_text_label->append_text("Failed to expand map because std::unexpected\n");
        }
    } else if (args[1] == "u") {
        Vector2i absolute_offset{0, abs(amount)};
        Vector2i offset = ((amount > 0) - (amount < 0)) * absolute_offset;
        Vector2i cur_dimensions = m_level->m_curmap->m_dimensions;
        Vector2i new_dimensions = m_level->m_curmap->m_dimensions + offset;

        if (new_dimensions.x <= TILE_COUNT_X || new_dimensions.y <= TILE_COUNT_Y) {
            m_rich_text_label->append_text(String("Map cannot be resized smaller than ") + TILE_COUNT_X + String(" ") + TILE_COUNT_Y + String("\n"));
            return true;
        }

        if (auto map = MapData::load_bare_map(new_dimensions); map.has_value()) {
            const auto result = map.value();

            if (amount >= 0) {
                /* create empty space at the top */
                for (int j = offset.y; j < new_dimensions.y; j++) {
                    for (int i = 0; i < cur_dimensions.x; i++) {
                        result->m_tile_data[j][i] = m_level->m_curmap->m_tile_data[j - offset.y][i];
                    }
                }
            } else {
                /* chop off from the top */
                for (int j = absolute_offset.y; j < cur_dimensions.y; j++) {
                    for (int i = 0; i < cur_dimensions.x; i++) {
                        result->m_tile_data[j - absolute_offset.y][i] = m_level->m_curmap->m_tile_data[j][i];
                    }
                }
            }

            m_level->m_curmap = result;
            m_level->m_bounds = Rect2{
                0 + TINY, 
                0 + TINY, 
                static_cast<float>(new_dimensions.x * TILE_SIZE) - (2 * TINY), 
                static_cast<float>(new_dimensions.y * TILE_SIZE) - (2 * TINY)
            };

            m_rich_text_label->append_text("Map expanded\n");
        } else {
            m_rich_text_label->append_text("Failed to expand map because std::unexpected\n");
        }
    } else if (args[1] == "l") {
        Vector2i absolute_offset{abs(amount), 0};
        Vector2i offset = ((amount > 0) - (amount < 0)) * absolute_offset;
        Vector2i cur_dimensions = m_level->m_curmap->m_dimensions;
        Vector2i new_dimensions = m_level->m_curmap->m_dimensions + offset;

        if (new_dimensions.x <= TILE_COUNT_X || new_dimensions.y <= TILE_COUNT_Y) {
            m_rich_text_label->append_text(String("Map cannot be resized smaller than ") + TILE_COUNT_X + String(" ") + TILE_COUNT_Y + String("\n"));
            return true;
        }

        if (auto map = MapData::load_bare_map(new_dimensions); map.has_value()) {
            const auto result = map.value();

            if (amount >= 0) {
                /* create empty space at the left */
                for (int j = 0; j < cur_dimensions.y; j++) {
                    for (int i = offset.x; i < new_dimensions.x; i++) {
                        result->m_tile_data[j][i] = m_level->m_curmap->m_tile_data[j][i - offset.x];
                    }
                }
            } else {
                /* chop off from the left */
                for (int j = 0; j < cur_dimensions.y; j++) {
                    for (int i = absolute_offset.x; i < cur_dimensions.x; i++) {
                        result->m_tile_data[j][i - absolute_offset.x] = m_level->m_curmap->m_tile_data[j][i];
                    }
                }
            }

            m_level->m_curmap = result;
            m_level->m_bounds = Rect2{
                0 + TINY, 
                0 + TINY, 
                static_cast<float>(new_dimensions.x * TILE_SIZE) - (2 * TINY), 
                static_cast<float>(new_dimensions.y * TILE_SIZE) - (2 * TINY)
            };

            m_rich_text_label->append_text("Map expanded\n");
        } else {
            m_rich_text_label->append_text("Failed to expand map because std::unexpected\n");
        }
    } else if (args[1] == "r") {
        Vector2i absolute_offset{abs(amount), 0};
        Vector2i offset = ((amount > 0) - (amount < 0)) * absolute_offset;
        Vector2i cur_dimensions = m_level->m_curmap->m_dimensions;
        Vector2i new_dimensions = m_level->m_curmap->m_dimensions + offset;

        if (new_dimensions.x <= TILE_COUNT_X || new_dimensions.y <= TILE_COUNT_Y) {
            m_rich_text_label->append_text(String("Map cannot be resized smaller than ") + TILE_COUNT_X + String(" ") + TILE_COUNT_Y + String("\n"));
            return true;
        }

        if (auto map = MapData::load_bare_map(new_dimensions); map.has_value()) {
            const auto result = map.value();

            for (int j = 0; j < cur_dimensions.y; j++) {
                for (int i = 0; i < std::min(cur_dimensions.x, new_dimensions.x); i++) {
                    result->m_tile_data[j][i] = m_level->m_curmap->m_tile_data[j][i];
                }
            }

            m_level->m_curmap = result;
            m_level->m_bounds = Rect2{
                0 + TINY, 
                0 + TINY, 
                static_cast<float>(new_dimensions.x * TILE_SIZE) - (2 * TINY), 
                static_cast<float>(new_dimensions.y * TILE_SIZE) - (2 * TINY)
            };
            m_level->m_player->m_true_pos.x += amount * TILE_SIZE;

            m_rich_text_label->append_text("Map expanded\n");
        } else {
            m_rich_text_label->append_text("Failed to expand map because std::unexpected\n");
        }
    }

    return true;
}

void Console::handle_command(const String new_text) {
    m_line_edit->set_text("");
    if (m_rich_text_label->get_line_count() >= 1000) {
        m_rich_text_label->clear();
    }

    m_rich_text_label->append_text("> ");
    m_rich_text_label->append_text(new_text);
    m_rich_text_label->append_text("\n");

    const auto split = new_text.split(" ", false);
    if (split.size() == 0) return;
    
    const auto command = split[0];
    if (command == "help") {
        m_rich_text_label->append_text("Available commands:\n");
        m_rich_text_label->append_text("help - display this help message\n");
        m_rich_text_label->append_text("clear - clear the console\n");
        m_rich_text_label->append_text("expand - expand/shrink the map (not idiot proof)\n");
    } else if (command == "clear") {
        m_rich_text_label->clear();
    } else if (command == "expand") {
        if (!handle_expand_command(split)) {
            m_rich_text_label->append_text("Usage: expand [lb]u]p|[lb]d]own|[lb]l]eft|[lb]r]ight number\n");
        }
    } else {
        m_rich_text_label->append_text("Unknown command - type help for available commands\n");
    }
}

void Console::_input(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("ui_cancel")) {
        set_visible(false);
        get_viewport()->set_input_as_handled();
        get_tree()->set_pause(false);
    }
}
