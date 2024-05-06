#ifndef MAPDATA_H
#define MAPDATA_H

#include <memory>
#include <expected>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

using namespace godot;

class MapData {
public:
	struct tile {
		int m_tile_group;
		int m_variant;
	} **m_tile_data;

	MapData(String path, Vector2i dimensions, Vector2 start_pos, tile **tile_data);
    ~MapData();

	String m_path;
	Vector2i m_dimensions;
	Vector2 m_start_pos;

    static std::expected<std::unique_ptr<MapData>, Error> load_bare_map();
    static std::expected<std::unique_ptr<MapData>, Error> load_map(const String &path);
    Error save_map(const String &path);

private:
	static std::expected<std::unique_ptr<MapData>, Error> load_map_v1(const String &path, const Dictionary &dict);
};

#endif
