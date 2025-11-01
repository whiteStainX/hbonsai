# PLAN: Refactoring to a Layer-Stack Architecture

This document outlines the plan to refactor the project from a sequential scene manager to a concurrent, layer-based compositing architecture. This will achieve true independence between animations and provide a highly modular and configurable system.

## 1. Architectural Goal: The Layer Stack

We will replace the current `SceneManager` with a `LayerStack`. On every frame, the `LayerStack` will update and draw all active `Layer`s simultaneously. Each layer will be an independent module with its own state, logic, and dedicated drawing surface (`ncplane`).

This enables:
- **Concurrency:** Multiple animations (e.g., title and bonsai) can run at the same time.
- **Modularity:** Each visual component (title, bonsai, effects) is a self-contained `Layer`.
- **Configurability:** The application's layout and active components can be defined by the configuration.

## 2. Step-by-Step Refactoring Plan

### Step 1: Redefine the `Scene` interface to `Layer`

1.  **Rename and Modify the Interface:**
    -   Rename `include/hbonsai/scene.h` to `include/hbonsai/layer.h`.
    -   Rename the `Scene` class to `Layer`.
    -   Update its virtual methods:
        ```cpp
        class Layer {
        public:
            virtual ~Layer() = default;

            // Called when the layer is added to the stack.
            virtual void onAttach(ncplane* plane) { ncp_ = plane; }

            // Called every frame to update state.
            virtual void update(double dt) = 0;

            // Called every frame to draw to the layer's own plane.
            virtual void draw() = 0;

            // Signals to the stack that this layer can be removed.
            virtual bool isFinished() const { return false; }

        protected:
            ncplane* ncp_ = nullptr; // The layer's personal drawing surface
        };
        ```

### Step 2: Upgrade `SceneManager` to `LayerStack`

1.  **Rename and Refactor:**
    -   Rename `scenemanager.h` to `layerstack.h` and `scenemanager.cpp` to `layerstack.cpp`.
    -   Rename the `SceneManager` class to `LayerStack`.
2.  **Update Member Variables:**
    -   It will now hold a `std::vector<std::unique_ptr<Layer>>` instead of a `deque`.
3.  **Rewrite the `run()` loop:**
    -   The loop will no longer pop from a queue.
    -   On each iteration, it will loop through the *entire* vector of layers and call `layer->update(dt)`.
    -   After all updates, it will loop through the vector again and call `layer->draw()`.
    -   It will be responsible for creating an `ncplane` for each layer it manages and passing it to the layer's `onAttach` method.

### Step 3: Convert `TitleScene` and `BonsaiScene` to Layers

1.  **Refactor `TitleScene` to `TitleLayer`:**
    -   Rename the files and the class.
    -   Inherit from `Layer`.
    -   The `draw(Renderer&)` method becomes `draw()`. All drawing will use the `ncp_` member variable.
2.  **Refactor `BonsaiScene` to `BonsaiLayer`:**
    -   Rename the files and the class.
    -   Inherit from `Layer`.
    -   The `draw(Renderer&)` method becomes `draw()`. All drawing will use the `ncp_` member variable.
    -   The `Renderer` will no longer be passed around. Instead, the `Layer` will encapsulate its own drawing logic on its own plane.

### Step 4: Create a Global Effects Layer (Example: Cursor)

1.  **Create `EffectsLayer`:**
    -   Create `effects_layer.h` and `effects_layer.cpp`.
    -   The `EffectsLayer` class will inherit from `Layer`.
2.  **Implement Blinking Logic:**
    -   Its `update(dt)` method will manage the timing for a blinking cursor.
    -   Its `draw()` method will get the current mouse coordinates from `notcurses` and draw a cursor character on its plane if the blink logic determines it's visible.
    -   Its plane will need to be transparent so the layers below are visible.

### Step 5: Update Configuration for Layouts

1.  **Add Layout Properties to Config:**
    -   The `TitleConfig` and `BonsaiConfig` structs will get new properties to define their plane's size and position (e.g., `int y, x, rows, cols`). We can also support percentages of the screen.
2.  **Update `main.cpp`:**
    -   The main function will now be responsible for reading this layout configuration.
    -   When creating layers, it will use the layout config to create the appropriate `ncplane` for each layer before adding it to the `LayerStack`.

### Step 6: Finalize `main.cpp`

1.  **Tie Everything Together:**
    -   The `main` function will be updated to use the new class names.
    -   It will create the `LayerStack`.
    -   It will create the `TitleLayer`, `BonsaiLayer`, and conditionally the `EffectsLayer` based on the config.
    -   It will add these layers to the stack.
    -   It will call `layerStack.run()`.

## 3. Expected Outcome

-   **True Independence:** The `TitleLayer` and `BonsaiLayer` will be able to run concurrently, each in its own region of the screen, without any knowledge of the other.
-   **Enhanced Modularity:** Adding a new visual element is as simple as creating a new class that inherits from `Layer` and adding it to the stack in `main`.
-   **Centralized Effects:** Global effects like a blinking cursor are handled in one place (`EffectsLayer`) and can be toggled via a single configuration flag.
-   **Declarative Layout:** The screen layout will be defined in the configuration, making the application highly flexible and easy to reconfigure.