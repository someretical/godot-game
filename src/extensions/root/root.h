#ifndef ROOT_H
#define ROOT_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource_preloader.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/templates/vector.hpp>

// In terms of tiles
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 12;
const int TILE_COUNT_X = MAP_WIDTH + 1;
const int TILE_COUNT_Y = MAP_HEIGHT + 1;
const int TILE_SIZE = 16;

namespace godot {

namespace priorities {

enum PhysicsProcessingPriority {
	Player = 10,
	Collectibles = 20,
	Mobs = 30,
	Tiles = 40
};

enum ProcessingPriority {
	Player = 10,
	Collectibles = 20,
	Mobs = 30,
	Tiles = 40
};

}

struct MapData {
	Vector2i dimensions;
	Vector2i start_pos;
	int **tile_data;
};

class Root : public Node {
	GDCLASS(Root, Node)

protected:
	static void _bind_methods();

public:
	Root();
	~Root();

	void _ready() override;
	void _physics_process(double delta) override;

	RandomNumberGenerator *m_rng;
	ResourcePreloader *m_player_preloader;
	ResourcePreloader *m_tile_preloader;
	Vector2 m_camera_pos;
	Vector2 m_camera_vel;
	Camera2D *m_camera;
	Rect2i m_camera_bounds;
	Node *m_map_node;
	Node *m_tiles_node;
	Node *m_collectables_node;
	Node *m_mobs_node;
	MapData m_curmap;
};

}

#endif