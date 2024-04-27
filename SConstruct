#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/extensions"])

library_names = [
    f"lib{d}"
    for d in os.listdir("src/extensions")
    if os.path.isdir("src/extensions/" + d)
]

for libname in library_names:
    name = libname[3:]

    register_types_cpp = f"""
#include "register_types.h"

#include "{name}.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_{libname}(ModuleInitializationLevel p_level) {{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {{
        return;
    }}

    ClassDB::register_class<{name.title()}>();
}}

void uninitialize_{libname}(ModuleInitializationLevel p_level) {{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {{
        return;
    }}
}}

extern "C" {{
GDExtensionBool GDE_EXPORT {libname}_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {{
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_{libname});
    init_obj.register_terminator(uninitialize_{libname});
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}}
}}
    """.strip()

    register_types_h = f"""
#ifndef {libname.upper()}_REGISTER_TYPES_H
#define {libname.upper()}_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_{libname}(ModuleInitializationLevel p_level);
void uninitialize_{libname}(ModuleInitializationLevel p_level);

#endif // {libname.upper()}_REGISTER_TYPES_H
    """.strip()

    extensionfile = f"""
[configuration]

entry_symbol = "{libname}_init"
compatibility_minimum = "4.2"

[libraries]

macos.debug = "res://src/scenes/bin/{libname}.macos.template_debug.framework"
macos.release = "res://src/scenes/bin/{libname}.macos.template_release.framework"
windows.debug.x86_32 = "res://src/scenes/bin/{libname}.windows.template_debug.x86_32.dll"
windows.release.x86_32 = "res://src/scenes/bin/{libname}.windows.template_release.x86_32.dll"
windows.debug.x86_64 = "res://src/scenes/bin/{libname}.windows.template_debug.x86_64.dll"
windows.release.x86_64 = "res://src/scenes/bin/{libname}.windows.template_release.x86_64.dll"
linux.debug.x86_64 = "res://src/scenes/bin/{libname}.linux.template_debug.x86_64.so"
linux.release.x86_64 = "res://src/scenes/bin/{libname}.linux.template_release.x86_64.so"
linux.debug.arm64 = "res://src/scenes/bin/{libname}.linux.template_debug.arm64.so"
linux.release.arm64 = "res://src/scenes/bin/{libname}.linux.template_release.arm64.so"
linux.debug.rv64 = "res://src/scenes/bin/{libname}.linux.template_debug.rv64.so"
linux.release.rv64 = "res://src/scenes/bin/{libname}.linux.template_release.rv64.so"
android.debug.x86_64 = "res://src/scenes/bin/{libname}.android.template_debug.x86_64.so"
android.release.x86_64 = "res://src/scenes/bin/{libname}.android.template_release.x86_64.so"
android.debug.arm64 = "res://src/scenes/bin/{libname}.android.template_debug.arm64.so"
android.release.arm64 = "res://src/scenes/bin/{libname}.android.template_release.arm64.so"
    """.strip()

    with open(f"src/extensions/{name}/register_types.cpp", "w") as f:
        f.write(register_types_cpp)

    with open(f"src/extensions/{name}/register_types.h", "w") as f:
        f.write(register_types_h)

    with open(f"src/scenes/bin/{name}.gdextension", "w") as f:
        f.write(extensionfile)

    libpath = (
        "src/scenes/bin/{}.{}.{}.framework/{}.{}.{}".format(
            libname,
            env["platform"],
            env["target"],
            libname,
            env["platform"],
            env["target"],
        )
        if env["platform"] == "macos"
        else "src/scenes/bin/{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])
    )

    Default(env.Library(target=libpath, source=Glob(f"src/extensions/{name}/*.cpp")))
