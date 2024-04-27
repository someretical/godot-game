#ifndef LIBTILE_REGISTER_TYPES_H
#define LIBTILE_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_libtile(ModuleInitializationLevel p_level);
void uninitialize_libtile(ModuleInitializationLevel p_level);

#endif // LIBTILE_REGISTER_TYPES_H