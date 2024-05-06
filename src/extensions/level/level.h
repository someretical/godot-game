#ifndef LEVEL_H
#define LEVEL_H

#include "mapdata.h"

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/resource_preloader.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class Player;
class Brush;

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
constexpr float TINY = 0.1f;

/* Lower values are processed first */
enum class PhysicsProcessingPriority {
	Player = 10,
	Collectibles = 20,
	Mobs = 30,
	Tiles = 40
};

enum class ProcessingPriority {
	Player = 10,
	SkidCloud = 11,
	Collectibles = 20,
	Mobs = 30,
	Tiles = 40
};

/* Larger values are draw on top */
enum class ZIndex {
	EditorBrush = 60,
	SkidCloud = 50,
	Player = 40,
	Mobs = 30,
	Collectibles = 20,
	Tiles = 10
};

class Level : public Node2D {
	GDCLASS(Level, Node2D)

protected:
	static void _bind_methods();

public:
	Level();
	~Level();

	void _input(const Ref<InputEvent> &event) override;
	void _ready() override;

	void update_camera();
	void set_camera_pos(const Vector2 pos);
	Vector2 get_camera_pos() const;

	void handle_editor_input(const Ref<InputEvent> &event);
	Error import_map_inplace(const String &path);
	Error export_current_map(const String &path);

	RandomNumberGenerator *m_rng;
	ResourcePreloader *m_tile_preloader;
	std::unique_ptr<MapData> m_curmap;
	Vector2 m_camera_pos;
	Player *m_player;
	Camera2D *m_camera;
	Rect2 m_bounds;
	Node *m_map_node;
	Node *m_tiles_node;
	Node *m_collectables_node;
	Node *m_mobs_node;
	Node *m_particles_node;
	struct editor {
		Brush *m_brush;
		bool m_enabled;
	} m_editor;
};

}

#endif