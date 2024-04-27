#ifndef LIBTEMPLATE_REGISTER_TYPES_H
#define LIBTEMPLATE_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_libtemplate(ModuleInitializationLevel p_level);
void uninitialize_libtemplate(ModuleInitializationLevel p_level);

#endif // LIBTEMPLATE_REGISTER_TYPES_H