#ifndef LIBLEVEL_REGISTER_TYPES_H
#define LIBLEVEL_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_liblevel(ModuleInitializationLevel p_level);
void uninitialize_liblevel(ModuleInitializationLevel p_level);

#endif // LIBLEVEL_REGISTER_TYPES_H