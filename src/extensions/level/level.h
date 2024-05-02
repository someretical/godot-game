#ifndef LEVEL_H
#define LEVEL_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/resource_preloader.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class Player;

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

struct MapData {
	Vector2i dimensions;
	Vector2i start_pos;
	int **tile_data;
};

class Level : public Node2D {
	GDCLASS(Level, Node2D)

protected:
	static void _bind_methods();

public:
	Level();
	~Level();

	void _ready() override;
	void _physics_process(double delta) override;

	void update_camera();
	void set_camera_pos(const Vector2 pos);
	Vector2 get_camera_pos() const;

	RandomNumberGenerator *m_rng;
	ResourcePreloader *m_tile_preloader;
	Vector2 m_camera_pos;
	Player *m_player;
	Camera2D *m_camera;
	Rect2 m_bounds;
	Node *m_map_node;
	Node *m_tiles_node;
	Node *m_collectables_node;
	Node *m_mobs_node;
	Node *m_particles_node;
	MapData m_curmap;
};

}

#endif