# Resourcemod

### How to build
1. `git clone --recurse-submodules git@github.com:ResourceMod/resourcemod.git`
2. Set environment variables HL2SDK and MMS_PATH (full paths)
3. Open AMBuildScript file and replace values in additional_includes with your full paths
4. Build v8 engine and put `v8_libbase.lib`, `v8_libplatform.lib` and `v8_monolith.lib` to ./lib folder
5. `make configure`
6. `make build`

### How to build v8
1. Install visual studio 2020
2. Download Windows SDK [here](https://download.microsoft.com/download/3/6/3/36301F10-B142-46FA-BE8C-728ECFD62EA5/windowssdk/winsdksetup.exe)
3. Download depot_tools [here](https://storage.googleapis.com/chrome-infra/depot_tools.zip)
4. Add depot_tools to your PATH environment variable (ex: E:/v8/depot_tools)
5. Set environment variable `DEPOT_TOOLS_WIN_TOOLCHAIN` to 0
6. Set environment variable `GYP_MSVS_VERSION` to 2020
7. Install python3
8. Install git
9. Get source code of v8 by `fetch v8`
10. `cd v8`
11. `python tools/dev/v8gen.py x64.release`
12. Open `out.gn\x64.release\args.gn` file and replace everything with values below
```
dcheck_always_on = false
is_debug = false
target_cpu = "x64"
is_component_build = false
v8_monolithic = true
v8_use_external_startup_data = false
v8_static_library = true
is_clang = false
```
13. Run build with `ninja -C out.gn/x64.release`


### How to build external runtime
1. `git clone --recurse-submodules git@github.com:ResourceMod/resourcemod.git`
2. run cmake with external_runtime target

### License
Current project is under GPL-3.0 license.