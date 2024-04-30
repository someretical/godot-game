# Setup

Currently only windows with MSVC is supported :skull:

**VS Code must be opened from the VS developer command prompt to ensure all the build tools are loaded into the environment!!!**

## Cloning
```
> git clone --recurse-submodules this-repo
```

## Building the C++ bindings
```
this-repo> python -m venv .venv
this-repo> pip install scons
this-repo> cd godot-cpp
this-repo\godot-cpp> scons platform=windows
```

## Building extensions

### Debug
```
this-repo> scons platform=windows
```

### Release
```
this-repo> scons platform=windows target=template_release
```