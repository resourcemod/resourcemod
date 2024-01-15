# Руководство по сборке на английском языке

Работает как в Windows, так и в Linux (если вы знаете, что делаете)

### Как построить
1. запустите в cmd или терминале `git clone --recurse-submodules git@github.com:ResourceMod/resourcemod.git`, если у вас есть git.
Если у вас нет git, скачайте его [здесь](https://git-scm.com/)
2. Установите переменные среды HL2SDK и MMS_PATH (полные пути).
3. Откройте файл AMBuildScript и замените значения в extra_includes полными путями.
4. Соберите движок v8 и поместите `v8_libbase.lib`, `v8_libplatform.lib` и `v8_monolith.lib` в папку ./lib.

!! ЕСЛИ У ВАС НЕТ КОМАНДЫ «make», ПРОЧТИТЕ [ЭТО](https://medium.com/@samsorrahman/how-to-run-a-makefile-in-windows-b4d115d7c516) !!

5. запустите `make configure`
6. запустите make build

### Как собрать v8
1. Установите Visual Studio 2020 [здесь](https://my.visualstudio.com/Downloads) (любая версия, если вам нужна бесплатная версия, используйте сообщество)
2. Загрузите Windows SDK [здесь] (https://download.microsoft.com/download/3/6/3/36301F10-B142-46FA-BE8C-728ECFD62EA5/windowssdk/winsdksetup.exe)
3. Загрузите depot_tools [здесь](https://storage.googleapis.com/chrome-infra/depot_tools.zip)
4. Добавьте depot_tools в переменную среды PATH (например: E:/v8/depot_tools).
5. Установите для переменной среды `DEPOT_TOOLS_WIN_TOOLCHAIN` значение 0.
6. Установите для переменной среды `GYP_MSVS_VERSION` значение 2020.
7. Установите Python 3.x [здесь](https://www.python.org/) (python3).
8. Получите исходный код v8, выполнив команду «fetch v8».
9. `cd v8`
10. `инструменты Python/dev/v8gen.py x64.release`
11. Откройте файл `out.gn\x64.release\args.gn` и замените все значениями ниже.
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
13. Запустите сборку с помощью `ninja -C out.gn/x64.release`


### Как создать внешнюю среду выполнения
1. запустите `git clone --recurse-submodules git@github.com:ResourceMod/resourcemod.git`
2. запустите cmake с целью external_runtime
