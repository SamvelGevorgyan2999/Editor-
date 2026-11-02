# Linux Installation & Build Guide

This project is written in **C++17** and uses **Qt 6** (`Core`, `Gui`,
`Widgets`, and `PrintSupport`) and **CMake 3.16+**.

## 1. Prerequisites

- CMake 3.16 or newer
- A C++17 compiler (GCC 9+ or Clang 10+)
- Qt 6.3 or newer, including the Widgets and PrintSupport development files
- Ninja (optional; recommended for faster builds)

## 2. Install the toolchain

### Debian / Ubuntu

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build qt6-base-dev qt6-base-dev-tools
```

### Fedora

```bash
sudo dnf install gcc-c++ cmake ninja-build qt6-qtbase-devel
```

### Arch Linux

```bash
sudo pacman -S --needed base-devel cmake ninja qt6-base
```

Package names and available Qt versions vary across distributions. Make sure
the installed Qt version is 6.3 or newer.

## 3. Configure and build

From the project root:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

If Qt is installed in a non-standard location, specify its installation
prefix:

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build build
```

## 4. Run

```bash
./build/AdvancedTextEditor
```

## 5. Run the tests

```bash
ctest --test-dir build --output-on-failure
```

## 6. Install

Install the application under a chosen prefix (for example, `/usr/local`):

```bash
cmake --install build --prefix /usr/local
```

## Troubleshooting

- **`Could NOT find Qt6`** — install the Qt 6 development package for your
  distribution, or set `CMAKE_PREFIX_PATH` to the Qt installation prefix.
- **PDF export or printing is unavailable** — ensure the Qt 6 PrintSupport
  development files are installed.
- **The application cannot find Qt libraries at runtime** — install the
  distribution's Qt 6 runtime packages or configure the dynamic linker to
  include the Qt library directory.
- **GNOME settings schema error mentioning `antialiasing`** — the application
  uses Qt's generic platform theme by default on Linux to avoid incompatible
  GNOME settings-daemon schemas. To explicitly select a different Qt platform
  theme, set `QT_QPA_PLATFORMTHEME` before launching.
