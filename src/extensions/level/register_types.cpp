#include "register_types.h"

#include "level.h"
#include "tile.h"
#include "player.h"
#include "skidcloud.h"
#include "brush.h"
#include "console.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_liblevel(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<Level>();
    ClassDB::register_class<Tile>();
    ClassDB::register_class<Player>();
    ClassDB::register_class<SkidCloud>();
    ClassDB::register_class<Brush>();
    ClassDB::register_class<Console>();
}

void uninitialize_liblevel(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT liblevel_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_liblevel);
    init_obj.register_terminator(uninitialize_liblevel);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
