# hbonsai

A C++ implementation of `cbonsai` using the `notcurses` library.

## Dependencies

- **CMake** (version 3.16 or higher)
- **A C++20 compatible compiler** (e.g., GCC, Clang)
- **Notcurses** library. Please install it using your system's package manager.
  - **On Arch Linux:** `sudo pacman -S notcurses`
  - **On Debian/Ubuntu:** `sudo apt-get install libnotcurses-dev`
  - **On macOS (Homebrew):** `brew install notcurses`

## Building

This project uses CMake.

1.  **Clone the repository:**

    ```bash
    git clone https://your-repo-url/hbonsai.git
    cd hbonsai
    ```

2.  **Create a build directory:**

    ```bash
    mkdir build
    cd build
    ```

3.  **Configure and build:**

    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ```

4.  **Run:**

    ```bash
    ./hbonsai [options]
    ```

## Usage

`hbonsai` mirrors the command-line interface of the original `cbonsai` reference implementation. All options can be discovered via `--help`:

```bash
./hbonsai --help
```

Key flags include:

- `-l, --live` – Grow the tree live, showing every step. Combine with `-t, --time` to control the delay between steps.
- `-i, --infinite` – Continuously grow new trees. Combine with `-w, --wait` to set the pause between trees.
- `-S, --screensaver` – Shortcut for live + infinite modes and quits on keypress. Automatically enables saving/loading progress.
- `-m, --message=STR` – Display a custom message alongside the tree.
- `-b, --base=INT` – Choose the ASCII-art pot/base (0 disables it).
- `-c, --leaf=LIST` – Provide a comma-separated list of leaf glyphs (UTF-8 supported).
- `-k, --color=LIST` – Provide four comma-separated 0–255 color indexes for dark leaves, dark wood, light leaves, and light wood.
- `-M, --multiplier=INT` – Control branching density.
- `-L, --life=INT` – Control overall growth length.
- `-p, --print` – Print the final tree to the terminal buffer before exiting.
- `-s, --seed=INT` – Seed the RNG deterministically.
- `-W, --save[=FILE]` – Persist progress (defaults to `$XDG_CACHE_HOME/cbonsai` or `$HOME/.cache/cbonsai`).
- `-C, --load[=FILE]` – Restore a saved seed/branch count (same defaults as `--save`).
- `-v, --verbose` – Increase verbosity.
- `-h, --help` – Display the full help text.

## Project Structure

- `src/`: Contains the source code.
  - `main.cpp`: The main entry point of the application.
  - `bonsai/`: Core logic for generating the bonsai tree.
  - `config/`: Handles configuration and command-line argument parsing.
  - `renderer/`: Responsible for rendering the tree and UI to the terminal via notcurses.
  - `title/`: For displaying titles and effects.
- `include/hbonsai/`: Contains the header files.
- `tests/`: Contains tests for the project.
- `CMakeLists.txt`: The main CMake build script.
