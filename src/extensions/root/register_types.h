#ifndef LIBROOT_REGISTER_TYPES_H
#define LIBROOT_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_libroot(ModuleInitializationLevel p_level);
void uninitialize_libroot(ModuleInitializationLevel p_level);

#endif // LIBROOT_REGISTER_TYPES_H