#include "mapdata.h"
#include "level.h"

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

const int CURRENT_VERSION = 1;

using namespace godot;

MapData::MapData(String path, Vector2i dimensions, Vector2 start_pos, MapData::tile **tile_data) : m_path{path}, m_dimensions{dimensions}, m_start_pos{start_pos}, m_tile_data{tile_data} {}

MapData::~MapData() {
    delete[] m_tile_data[0];
    delete[] m_tile_data;
}

void MapData::copy_tile_data(const std::shared_ptr<MapData> &src, const std::shared_ptr<MapData> &dst, const Vector2i &offset) {
    const Vector2i start_inc{offset.x, offset.y};
    const Vector2i end_excl{offset.x + src->m_dimensions.x, offset.y + src->m_dimensions.y};

    for (int j = start_inc.y; j < end_excl.y; j++) {
        for (int i = start_inc.x; i < end_excl.x; i++) {
            dst->m_tile_data[j][i] = src->m_tile_data[j - offset.y][i - offset.x];
        }
    }
}

std::expected<std::shared_ptr<MapData>, Error> MapData::load_bare_map(const Vector2i &dimensions) {
    const auto tile_data = new MapData::tile *[dimensions.y];
    if (!tile_data) return std::unexpected(FAILED);
    memset(tile_data, 0, dimensions.y * sizeof(MapData::tile *));

    const auto temp = new MapData::tile[dimensions.x * dimensions.y];
    if (!temp) {
        delete[] tile_data;
        return std::unexpected(FAILED);
    }
    memset(temp, -1, dimensions.x * dimensions.y * sizeof(MapData::tile));

    for (int j = 0; j < dimensions.y; j++) {
        tile_data[j] = temp + j * dimensions.x;
    }

    return std::make_shared<MapData>("", dimensions, Vector2{0, 0}, tile_data);
}

std::expected<std::shared_ptr<MapData>, Error> MapData::load_map_v1(const String &path, const Dictionary &dict) {
    /* validate file contents */
    const auto var = dict.get("dimensions", Array{});
    if (var.get_type() != Variant::Type::ARRAY) return std::unexpected(FAILED);
    const Array var_array = var;
    if (
        var_array.size() != 2 
        || var_array[0].get_type() != Variant::Type::FLOAT 
        || var_array[1].get_type() != Variant::Type::FLOAT
    ) return std::unexpected(FAILED);

    const Vector2i dimensions{var_array[0], var_array[1]};
    if (
        dimensions[0] < SCREEN_TILE_WIDTH
        || dimensions[1] < SCREEN_TILE_HEIGHT
    ) return std::unexpected(FAILED);

    const auto var2 = dict.get("start_pos", Array{});
    if (var2.get_type() != Variant::Type::ARRAY) return std::unexpected(FAILED);
    const Array var2_array = var2;
    if (
        var2_array.size() != 2
        || var2_array[0].get_type() != Variant::Type::FLOAT
        || var2_array[1].get_type() != Variant::Type::FLOAT
    ) return std::unexpected(FAILED);
    const Vector2 start_pos{var2_array[0], var2_array[1]};

    const auto var3 = dict.get("tile_data", Array{});
    if (var3.get_type() != Variant::Type::ARRAY) return std::unexpected(FAILED);
    const Array tile_array = var3;
    if (tile_array.size() != dimensions.y) return std::unexpected(FAILED);

    for (int j = 0; j < dimensions.y; j++) {
        const auto var4 = tile_array[j];
        if (var4.get_type() != Variant::Type::ARRAY) return std::unexpected(FAILED);
        const Array row_array = var4;
        if (row_array.size() != dimensions.x) return std::unexpected(FAILED);

        for (int i = 0; i < dimensions.x; i++) {
            const auto var5 = row_array[i];
            if (var5.get_type() != Variant::Type::ARRAY) return std::unexpected(FAILED);
            const Array var5_array = var5;
            if (
                var5_array.size() != 2
                || var5_array[0].get_type() != Variant::Type::FLOAT
                || var5_array[1].get_type() != Variant::Type::FLOAT
            ) return std::unexpected(FAILED);

            const int group = var5_array[0];
            const int variant = var5_array[1];

            switch (group) {
                case -1:
                    if (variant != -1) return std::unexpected(FAILED);
                    break;
                case 0:
                    if (variant < 0 || variant >= 9) return std::unexpected(FAILED);
                    break;
                case 1:
                    if (variant < 0 || variant >= 6) return std::unexpected(FAILED);
                    break;
                case 2:
                    if (variant != 0) return std::unexpected(FAILED);
                    break;
                case 3:
                    if (variant != 0) return std::unexpected(FAILED);
                    break;
                case 4:
                    if (variant < 0 || variant >= 4) return std::unexpected(FAILED);
                    break;
                case 5:
                    if (variant < 0 || variant >= 4) return std::unexpected(FAILED);
                    break;
                default:
                    return std::unexpected(FAILED);
            }
        }
    }

    /* allocate contents */
    const auto tile_data = new MapData::tile *[dimensions.y];
    if (!tile_data) return std::unexpected(FAILED);
    memset(tile_data, 0, dimensions.y * sizeof(MapData::tile *));

    const auto temp = new MapData::tile[dimensions.x * dimensions.y];
    if (!temp) {
        delete[] tile_data;
        return std::unexpected(FAILED);
    }
    memset(temp, -1, dimensions.x * dimensions.y * sizeof(MapData::tile));
    for (int j = 0; j < dimensions.y; j++) {
        tile_data[j] = temp + j * dimensions.x;
    }

    for (int j = 0; j < dimensions.y; j++) {
        const Array row_array = tile_array[j];

        for (int i = 0; i < dimensions.x; i++) {
            const Array tile = row_array[i];
            const int group = tile[0];
            const int variant = tile[1];

            tile_data[j][i].m_tile_group = group;
            tile_data[j][i].m_variant = variant;
        }
    }

    return std::make_shared<MapData>(path, dimensions, start_pos, tile_data);
}

std::expected<std::shared_ptr<MapData>, Error> MapData::load_map(const String &path) {
    const auto file = FileAccess::open(path, FileAccess::READ);
    const auto err = FileAccess::get_open_error();
    if (err != OK) return std::unexpected(err);

    const auto var1 = JSON::parse_string(file->get_as_text());
    if (var1.get_type() != Variant::Type::DICTIONARY) return std::unexpected(FAILED);
    const Dictionary dict = var1;

    const auto var2 = dict.get("version", -1);
    if (var2.get_type() != Variant::Type::FLOAT) return std::unexpected(FAILED);
    const int version = var2;

    switch (version) {
        case CURRENT_VERSION: return load_map_v1(path, dict);
        default: return std::unexpected(FAILED);
    }
}

Error MapData::save_map(const String &path) {
    Variant root = Dictionary{};
    root.set("version", CURRENT_VERSION);
    root.set("dimensions", Array{PackedInt32Array{m_dimensions.x, m_dimensions.y}});
    root.set("start_pos", Array{PackedFloat32Array{m_start_pos.x, m_start_pos.y}});

    Array tile_data{};
    tile_data.resize(m_dimensions.y);

    for (int j = 0; j < m_dimensions.y; j++) {
        Array row_data{};
        row_data.resize(m_dimensions.x);

        for (int i = 0; i < m_dimensions.x; i++) {
            row_data[i] = PackedInt32Array{m_tile_data[j][i].m_tile_group, m_tile_data[j][i].m_variant};
        }
        tile_data[j] = row_data;
    }

    root.set("tile_data", tile_data);

    const auto file = FileAccess::open(path, FileAccess::WRITE);
    const auto err = FileAccess::get_open_error();
    if (err != OK) return err;

    file->store_string(JSON::stringify(root, "\t", true, false));
    file->close();
    return OK;
}