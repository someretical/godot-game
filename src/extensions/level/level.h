#ifndef LEVEL_H
#define LEVEL_H

#include "mapdata.h"

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/resource_preloader.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>

namespace godot {

class Player;
class Brush;
class Console;
class Marker;

constexpr int ceil_div(int numerator, int denominator) {
    return (numerator + denominator - 1) / denominator;
}

constexpr int SCREEN_WIDTH = 480;
constexpr int SCREEN_HEIGHT = 360;
constexpr int SCREEN_ZOOM = 1;
constexpr int CAMERA_WIDTH = SCREEN_WIDTH / SCREEN_ZOOM;
constexpr int CAMERA_HEIGHT = SCREEN_HEIGHT / SCREEN_ZOOM;
constexpr int TILE_SIZE = 32;
constexpr int HALF_TILE = TILE_SIZE / 2;
constexpr int SCREEN_TILE_WIDTH = CAMERA_WIDTH / TILE_SIZE;
constexpr int SCREEN_TILE_HEIGHT = ceil_div(CAMERA_HEIGHT, TILE_SIZE);
constexpr int TILE_COUNT_X = SCREEN_TILE_WIDTH + 1;
constexpr int TILE_COUNT_Y = SCREEN_TILE_HEIGHT + 1;
constexpr float TINY = 1.0f;

constexpr int GAME_LAYER = 0;
constexpr int HUD_LAYER = 1;

/* Lower values are processed first */
enum class PhysicsProcessingPriority {
	Player = 10,
	Collectibles = 20,
	Mobs = 30,
	Tiles = 40
};

enum class ProcessingPriority {
	Level = 0,
	Player = 10,
	SkidCloud = 11,
	Collectibles = 20,
	Mobs = 30,
	Tiles = 40
};

/* Larger values are draw on top */
enum class ZIndex {
	EditorBrush = 500,
	SkidCloud = 400,
	Player = 300,
	Mobs = 200,
	Collectibles = 100,
	Markers = 1,
	Tiles = 0
};

class Level : public Node2D {
	GDCLASS(Level, Node2D)

protected:
	static void _bind_methods();

public:
	Level();
	~Level();

	void _unhandled_input(const Ref<InputEvent> &event) override;
	void _ready() override;
	void _process(double delta) override;

	void update_camera();
	void set_camera_pos(const Vector2 pos);
	Vector2 get_camera_pos() const;

	bool handle_console_open(const Ref<InputEvent> &event);
	bool handle_editor_toggle(const Ref<InputEvent> &event);
	Error import_map_inplace(const String &path);
	Error export_current_map(const String &path);

	int get_tile_frame_mod4() const;

	RandomNumberGenerator *m_rng;
	ResourcePreloader *m_tile_preloader;
	std::shared_ptr<MapData> m_curmap;
	Vector2 m_camera_true_pos;
	CanvasLayer *m_hud_layer;
	CanvasLayer *m_game_layer;
	Console *m_console;
	Player *m_player;
	Camera2D *m_camera;
	Rect2 m_bounds;
	Node *m_tiles_node;
	Node *m_collectables_node;
	Node *m_mobs_node;
	Node *m_particles_node;
	double m_time;

	struct editor {
		Brush *m_brush;
		Marker *m_start_pos;
		bool m_enabled;
	} m_editor;
};

}

#endif