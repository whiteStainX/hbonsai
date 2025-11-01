# Refactoring and Design Improvement Plan

This document outlines a plan to refactor the `hbonsai` project to a more modular, extensible, and configurable architecture. This plan is based on the results of a code review and addresses the new requirements for a separate title animation and higher modularity.

## 1. Architectural Goals

1.  **Scene-Based Architecture:** Transition from a single, hardcoded application loop in `main()` to a flexible, scene-based architecture. This will allow for different parts of the application (e.g., title, bonsai, credits) to exist as independent, self-contained modules.
2.  **Component-Based Configuration:** Break down the monolithic `Config` struct into smaller, component-specific configuration objects (e.g., `BonsaiConfig`, `TitleConfig`, `AppConfig`).
3.  **Decoupled Main Loop:** The main application loop should be completely agnostic of what is being animated or rendered. It will be driven by a `SceneManager` that handles transitions and updates.

## 2. Proposed Architecture: Scene Manager

We will implement a **Scene Manager** pattern. This involves the following components:

-   **`Scene` (Interface):** An abstract base class that defines the contract for any animatable scene. It will have the following methods:
    -   `virtual void update(double dt) = 0;` (To update animation state)
    -   `virtual void draw(Renderer& renderer) = 0;` (To render the current state)
    -   `virtual bool isFinished() const = 0;` (To signal completion to the SceneManager)

-   **Concrete `Scene` Implementations:**
    -   **`TitleScene`:** A new class for the title animation. It will manage its own state, animation logic, and rendering calls. It will be completely isolated from the bonsai logic.
    -   **`BonsaiScene`:** This class will encapsulate the logic currently in `main.cpp` for growing and rendering the bonsai tree.

-   **`SceneManager`:** A class that manages a queue of `Scene` objects. It will be responsible for:
    -   Running the main application loop.
    -   Calling `update()` and `draw()` on the current scene.
    -   Checking `isFinished()` and transitioning to the next scene in the queue.

## 3. Step-by-Step Refactoring Plan

### Step 1: Modularize Configuration ✅ Completed

1.  **Create Component Configs:**
    -   In `include/hbonsai/config.h`, break the `Config` struct into smaller structs:
        -   `BonsaiConfig`: Contains `lifeStart`, `multiplier`, `leaves`, `colors`, etc.
        -   `TitleConfig`: A new struct for title text, animation style, colors, etc.
        -   `AppConfig`: Contains application-level settings like `live`, `infinite`, `timeStep`, `screensaver`.
    -   The main `Config` struct will now be composed of these smaller structs.

2.  **Update Argument Parsing:**
    -   In `src/config/Config.cpp`, update the `parse_args` function to populate the new, nested configuration structs.

### Step 2: Implement the Scene Management System ✅ Completed

1.  **Create `Scene` Interface:**
    -   Create a new header file `include/hbonsai/scene.h`.
    -   Define the abstract `Scene` base class within this file.

2.  **Create `SceneManager`:**
    -   Create `include/hbonsai/scenemanager.h` and `src/scenemanager.cpp`.
    -   The `SceneManager` will have methods like `addScene(std::unique_ptr<Scene> scene)` and `run()`.
    -   The `run()` method will contain the main application loop that is currently in `main.cpp`.

### Step 3: Refactor `Bonsai` into `BonsaiScene` ✅ Completed

1.  **Create `BonsaiScene`:**
    -   Create `include/hbonsai/bonsai_scene.h` and `src/bonsai_scene.cpp`.
    -   This class will inherit from `Scene`.
    -   Move the bonsai-related logic from `main.cpp` into this class.
    -   The `BonsaiScene` will own the `Bonsai` object (the generator) and the vector of `TreePart`s.
    -   The `update()` method will handle the step-by-step generation in live mode.
    -   The `draw()` method will call the appropriate `Renderer` methods.

### Step 4: Create the New `TitleScene` ✅ Completed

1.  **Create `TitleScene`:**
    -   Create `include/hbonsai/title_scene.h` and `src/title_scene.cpp`.
    -   This class will inherit from `Scene`.
    -   It will contain all logic for animating and rendering the title. This fulfills the requirement for a clean, isolated, and portable animation.
    -   The `TitleConfig` struct will be passed to its constructor.

### Step 5: Simplify the `main()` Function ✅ Completed

1.  **Update `main.cpp`:**
    -   The `main()` function will become much simpler. Its only responsibilities will be:
        1.  Parse arguments into the `Config` object.
        2.  Initialize the `Renderer`.
        3.  Initialize the `SceneManager`.
        4.  Create instances of `TitleScene` and `BonsaiScene` (based on the config) and add them to the `SceneManager`.
        5.  Call `sceneManager.run()`.

## 4. Expected Outcome

-   **High Modularity:** Each part of the application (title, bonsai) is a self-contained `Scene`. Adding a new animation is as simple as creating a new class that implements the `Scene` interface.
-   **Clean Separation:** The bonsai generation logic (`Bonsai` class) is cleanly separated from the animation and rendering logic (`BonsaiScene` and `Renderer`).
-   **Configurability:** The configuration is neatly organized by component, making it easier to manage and extend.
-   **Flexibility:** The `SceneManager` can easily be configured to run scenes in any order, or even to create branching application flows.
