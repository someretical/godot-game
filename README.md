# Setup

Currently only windows with MSVC is supported :skull:

**VS Code must be opened from the VS developer command prompt to ensure all the build tools are loaded into the environment!!!**

## Cloning

```
> git clone --recurse-submodules this-repo
```

## Install scons

```
this-repo> python -m venv .venv
this-repo> pip install scons
```

## Building

First, replace the `program` value in `.vscode/launch.json` with the absolute path to your Godot executable.

The debug task can now be run. It will take a while the first time.

You can also run the release build task to generate an optimized version.
