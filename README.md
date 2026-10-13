# Graphics Engineer — тестовое задание «Вращающийся куб с правильной синхронизацией»

Bear Head Studio · Quake RTX на Apple Metal

Нарисуйте текстурированный вращающийся куб на Metal (трек Mac) или Vulkan 1.3 (трек без Mac) с depth-буфером, ресайзом окна и triple buffering uniform-данных. Главное в задании — корректная синхронизация CPU ↔ GPU без ожидания GPU в каждом кадре. Кроме кода, заполните короткую карту соответствий Vulkan → Metal.

Задание — в документе [`Тестовое_задание_Graphics_Engineer.docx`](Тестовое_задание_Graphics_Engineer.docx).

## Состав пакета

| Путь | Что это |
|---|---|
| `Тестовое_задание_Graphics_Engineer.docx` | задание: задача, входные данные, что сделать, как сдать |
| `vulkan-to-metal.md` | шаблон документа: часть А (8 понятий и раздел о синхронизации, обязательно), часть Б (по желанию) |
| `assets/cube.obj`, `assets/checker.png` | куб (24 вершины, 12 треугольников, UV, нормали) и шахматная текстура 512×512 с цветными маркерами углов |
| `starter/common/obj_loader.h`, `starter/common/math3d.h` | загрузчик OBJ и матрицы 4×4 (перспективу пишете вы) |
| `starter/metal/` | трек Mac: CMake + MetalKit на Objective-C++, окно и очистка экрана; места для вашего кода помечены `TODO`. Писать можно на Objective-C++ или на metal-cpp |
| `starter/vulkan/` | трек без Mac: CMake + Vulkan SDK + GLFW. Готовый запуск в `src/vk_base.cpp`: instance и валидация, устройство, swapchain, depth-изображение, пересоздание при ресайзе и сворачивании. Чтение PNG в память — `src/image_io.cpp` (`bhs::LoadRGBA8`, stb_image из `third_party/stb/`). Места для вашего кода помечены `TODO` в `src/main.cpp` |
| `tools/gen_assets.py` | генератор ассетов; запускать не нужно, ассеты уже в пакете |

## Как начать

Трек Mac: macOS 14+, любой Mac с поддержкой Metal, Command Line Tools (`xcode-select --install`). CMake 3.20+ в Command Line Tools не входит, его ставят отдельно: `brew install cmake`.

```sh
cmake -S starter/metal -B build && cmake --build build
MTL_DEBUG_LAYER=1 ./build/cube_metal
```

Без CMake трек Mac собирается одной командой из папки пакета:

```sh
clang++ -std=c++17 -fobjc-arc starter/metal/src/main.mm -o cube_metal \
  -DBHS_ASSET_DIR='"assets"' -DBHS_SHADER_PATH='"starter/metal/src/shaders.metal"' \
  -framework Metal -framework MetalKit -framework AppKit -framework QuartzCore
MTL_DEBUG_LAYER=1 ./cube_metal
```

Трек без Mac (Vulkan SDK 1.3+, CMake 3.20+; GLFW скачается сам):

```sh
cmake -S starter/vulkan -B build -DBHS_FETCH_GLFW=ON && cmake --build build
./build/cube_vulkan        # Windows: build\Debug\cube_vulkan.exe
```

В отладочной сборке `VK_LAYER_KHRONOS_validation` включается стартером, ошибки валидации выводятся в консоль.
