
### [Download with mod manager.](www.nexusmods.com/skyrimspecialedition/mods/123445)

# CommonLibSSE NG

Because this uses [CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG), it supports Skyrim SE, AE, GOG, and VR.

Hook IDs and offsets must still be found manually for each version.

## Skyrim 1.7.104 support

This tree pins [alandtse/CommonLibSSE-NG v11.0.0](https://github.com/alandtse/CommonLibSSE-NG/releases/tag/v11.0.0) at commit `94faaed0c60eddd8347767f2d4d29a97c93bde8c`. That release reads the format-5 Address Library database used by Skyrim 1.7.99 and 1.7.104 and marks the generated SKSE plugin declaration with the format-5 compatibility flag.

For Skyrim 1.7.104, install SKSE 2.3.1 and the matching `Data/SKSE/Plugins/versionlib-1-7-104-0.bin`. The Address Library file is a runtime dependency and is not included here.

The two inline hook sites and the three vtable slots used by Asset Doctor were statically checked against one Steam 1.7.104 executable. An in-game smoke test with Skyrim 1.7.104 and SKSE 2.3.1 confirmed plugin load, hook installation, and ImGui initialization. See [`docs/1.7.104-validation.md`](docs/1.7.104-validation.md) for the exact inputs and evidence boundary.

# Requirements

- [Visual Studio 2022](https://visualstudio.microsoft.com/) (_the free Community edition_)
- [`vcpkg`](https://github.com/microsoft/vcpkg)
  - 1. Clone the repository using git OR [download it as a .zip](https://github.com/microsoft/vcpkg/archive/refs/heads/master.zip)
  - 2. Go into the `vcpkg` folder and double-click on `bootstrap-vcpkg.bat`
  - 3. Edit your system or user Environment Variables and add a new one:
    - Name: `VCPKG_ROOT`  
      Value: `C:\path\to\wherever\your\vcpkg\folder\is`
  - The latest version of vcpkg needs a default repository defined in the json. If you're using an older version of vcpkg, simply delete the default repository definition in `vcpkg-configuration.json`

## Opening the project

Once you have Visual Studio 2022 installed, you can open this folder in basically any C++ editor, e.g. [VS Code](https://code.visualstudio.com/) or [CLion](https://www.jetbrains.com/clion/) or [Visual Studio](https://visualstudio.microsoft.com/)
- > _for VS Code, if you are not automatically prompted to install the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions, please install those and then close VS Code and then open this project as a folder in VS Code_

You may need to click `OK` on a few windows, but the project should automatically run CMake!

It will _automatically_ download [CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG) and everything you need to get started making your new plugin!

# Project setup

By default, when this project compiles it will output a `.dll` for your SKSE plugin into the `build/` folder.

If you want to configure this project to output your plugin files
into your Skyrim Special Edition's "`Data`" folder:

- Set the `SKYRIM_FOLDER` environment variable to the path of your Skyrim installation  
  e.g. `C:\Program Files (x86)\Steam\steamapps\common\Skyrim Special Edition`

If you want to configure this project to output your plugin files
into your "`mods`" folder:  
(_for Mod Organizer 2 or Vortex_)

- Set the `SKYRIM_MODS_FOLDER` environment variable to the path of your mods folder:  
  e.g. `C:\Users\<user>\AppData\Local\ModOrganizer\Skyrim Special Edition\mods`  
  e.g. `C:\Users\<user>\AppData\Roaming\Vortex\skyrimse\mods`


