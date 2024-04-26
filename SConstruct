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

# find all folders inside extensions
extensions = [
    d for d in os.listdir("src/extensions") if os.path.isdir("src/extensions/" + d)
]

library_sources = {}

for extension in extensions:
    library_sources[f"lib{extension}"] = Glob(f"src/extensions/{extension}/*.cpp")

for libname, libsources in library_sources.items():
    libpath = (
        "src/scenes/bin/{}.{}.{}.framework/{}.{}.{}".format(
            libname, env["platform"], env["target"], libname, env["platform"], env["target"]
        )
        if env["platform"] == "macos"
        else "src/scenes/bin/{}{}{}".format(
            libname, env["suffix"], env["SHLIBSUFFIX"]
        )
    )

    Default(
        env.Library(
            target=libpath,
            source=env.Object(source=libsources),
        )
    )
