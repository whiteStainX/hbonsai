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
    ./hbonsai
    ```

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
