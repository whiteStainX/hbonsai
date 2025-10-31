# hbonsai Design Document

This document outlines the software design and architecture for `hbonsai`, a modern C++ reimplementation of the original `cbonsai`.

## 1. Project Goals

- **Modern C++ Reimplementation:** Port the original C application to modern C++ (C++20), emphasizing object-oriented principles, RAII, and type safety.
- **Replace `ncurses` with `notcurses`:** Leverage the more advanced features of the `notcurses` library for a richer terminal UI.
- **Extensible Architecture:** Create a clean, decoupled architecture that allows for new features (e.g., title effects, new tree generators) to be added easily in the future.
- **Maintain Original Functionality:** Ensure all original command-line flags and core behaviors of `cbonsai` are preserved.

## 2. Core Architecture

The architecture is based on the Model-View-Controller (MVC) pattern to decouple the core logic from the user interface.

-   **Model: `Bonsai` Class**
    -   **Responsibility:** Encapsulates the state and logic for generating a bonsai tree. It is the "engine" of the application.
    -   **Implementation:** It contains the C++ translation of the `branch`, `setDeltas`, and `chooseString` functions from `ref.c`. It knows nothing about rendering or `notcurses`. Its sole output is a `std::vector<TreePart>`, which is a complete, render-agnostic representation of the tree.

-   **View: `Renderer` Class**
    -   **Responsibility:** Manages all terminal rendering via `notcurses`. It is the "view" of the application.
    -   **Implementation:** It consumes the `std::vector<TreePart>` from the `Bonsai` class and translates this data into `notcurses` drawing commands. It encapsulates all `notcurses` objects (`notcurses`, `ncplane`) and handles the drawing of the tree, the base, and the message.

-   **Controller: `main()` Function**
    -   **Responsibility:** Orchestrates the application flow. It initializes the components, runs the main application loop, and manages program state.
    -   **Implementation:** It parses command-line arguments to create a `Config` object, initializes the `Bonsai` and `Renderer`, and then, based on the `Config`, drives the application. It is responsible for the animation loop in live mode.

-   **Configuration: `Config` Struct**
    -   **Responsibility:** A simple data structure that holds all application settings. It is populated by the `parse_args` function.

## 3. Porting Strategy: C to C++

The transition from the original C codebase to C++ follows several key principles:

-   **Global State to Class Members:** C globals and structs that were passed around by pointer (`conf`, `myCounters`, `objects`) are now encapsulated as private member variables within the C++ classes. For example, the `counters` struct is now part of the `Bonsai` class.

-   **C Functions to C++ Methods:** Global C functions have been converted into class methods. For example:
    -   `branch`, `setDeltas`, `chooseString` are now private methods of the `Bonsai` class.
    -   `drawBase`, `drawMessage` are now private methods of the `Renderer` class.

-   **Manual Memory Management to RAII:** C's `malloc`/`free` are eliminated. C-style strings (`char*`) are replaced with `std::string`, and dynamically sized arrays are replaced with `std::vector`. This leverages the C++ standard library for automatic resource management, preventing memory leaks.

-   **Type Safety:** C-style enums (`enum branchType`) are converted to C++ `enum class` to provide stronger type-checking and avoid naming conflicts.

-   **Random Number Generation:** The C functions `srand` and `rand` have been replaced with the more robust and modern C++ `<random>` library (`std::mt19937`).

## 4. Live Mode Implementation

The `--live` flag is implemented by decoupling tree generation from rendering, as detailed below:

1.  **Generate First:** The `main` function first calls `bonsai.generate()` to create the *complete* vector of `TreePart` objects in memory.

2.  **Check Mode:** It then checks the `config.live` flag.

3.  **Live Mode Execution:** If `live` is true, the program iterates through the vector of `TreePart` objects. In each iteration, it:
    a.  Calls `renderer.drawLive(part)` to draw just one piece of the tree.
    b.  Calls `renderer.render()` to update the physical screen.
    c.  Pauses for the `config.timeStep` duration.

4.  **Static Mode Execution:** If `live` is false, the program calls `renderer.drawStatic(parts)` a single time to render the entire tree at once.

This design ensures that the core tree generation logic is identical for both modes, completely separating the generation algorithm from the animation logic.

## 5. Future Extensibility

This decoupled design makes it easy to add new features:

-   **New Title Effects:** A new title effect can be implemented entirely within a new `Title` class. The `Renderer` would then just need to be updated to call the new title's render method, with no changes to the `Bonsai` logic.
-   **Different Tree Generators:** A new type of tree could be created by writing a new class with a `generate()` method that returns a `std::vector<TreePart>`. The `main` function could then choose which generator to use based on a new command-line flag.