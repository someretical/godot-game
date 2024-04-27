#ifndef LIBPLAYER_REGISTER_TYPES_H
#define LIBPLAYER_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_libplayer(ModuleInitializationLevel p_level);
void uninitialize_libplayer(ModuleInitializationLevel p_level);

#endif // LIBPLAYER_REGISTER_TYPES_H