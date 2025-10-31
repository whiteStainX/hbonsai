# Analysis of Command-Line Flag Behavior

This document explains why command-line flags like `--live` currently have no effect on the application's behavior and provides a step-by-step guide to implement the required functionality.

## The Root Cause

The issue is not in parsing the flags, but in using them. The investigation confirms the following:

1.  **Flags are Parsed Correctly:** The argument parsing code in `src/config/Config.cpp` successfully uses `getopt_long` to identify flags like `--live` and correctly sets the `live` boolean to `true` in the `Config` struct.

2.  **Program Logic is Static:** The `main()` function in `src/main.cpp` follows a simple, static execution path:
    *   It calls `bonsai.grow(...)` to generate the *entire* tree data structure in memory.
    *   It then calls `renderer.draw(...)` to render the *complete* tree to the screen in a single frame.
    *   The value of `config.live` is never checked or used to alter this behavior.

In short, the program is only built to draw the final tree all at once. It lacks the animation loop required for live mode.

## Steps to Implement Live Mode

To make the `--live` flag work, the application's core logic must be refactored to support two modes: a static mode (draw at the end) and a live mode (draw step-by-step).

Here are the required code modifications:

### Step 1: Modify the `Bonsai` Class for Incremental Generation

The `Bonsai` class must be changed so it can generate the tree one piece at a time.

-   **File:** `include/hbonsai/bonsai.h`
-   **Action:** Change the `grow()` method to `generate()`. Instead of building a private list of tree parts, this method will now return the complete vector of `TreePart` objects. This allows the main loop to control the generation and rendering.

-   **File:** `src/bonsai/Bonsai.cpp`
-   **Action:** Implement the `generate()` method. This method will contain the core recursive logic from `ref.c` to build up and return a vector of all the tree parts.

### Step 2: Modify the `Renderer` Class for Incremental Drawing

The `Renderer` needs to be able to draw the whole tree at once, but also draw single pieces for live mode.

-   **File:** `include/hbonsai/renderer.h` and `src/renderer/Renderer.cpp`
-   **Action:** Create two distinct drawing methods:
    1.  `drawStatic(const std::vector<TreePart>& parts)`: A method that takes the full vector of tree parts and renders them all in one go.
    2.  `drawLive(const TreePart& part)`: A method that takes a single `TreePart` and renders just that piece to the screen.

### Step 3: Update `main.cpp` with the Main Animation Loop

This is the most critical change. The `main()` function must be updated to handle both live and static modes.

-   **File:** `src/main.cpp`
-   **Action:**
    1.  After initializing the `Bonsai` and `Renderer` objects, call `bonsai.generate()` to get the complete list of `TreePart` objects for the tree.
    2.  Check the `config.live` flag.
    3.  **If `true` (Live Mode):**
        *   Create a `for` loop to iterate through the vector of `TreePart` objects.
        *   Inside the loop, call `renderer.drawLive(part)` for each part.
        *   Call `renderer.render()` to update the screen.
        *   Sleep for the duration specified by `config.timeStep`.
    4.  **If `false` (Static Mode):**
        *   Call `renderer.drawStatic(parts)` once with the full vector of parts.
        *   Call `renderer.render()` to update the screen.
    5.  After either the loop finishes or the static draw is complete, call `renderer.wait()` to wait for user input before exiting.

---

## Reference C Functions from `ref.c`

These are the key functions from the original C code that need to be ported to C++ to implement the tree generation and live mode logic.

### `branch()`

This is the core recursive function that generates the tree. The `while (life > 0)` loop is the heart of the generation. In live mode, each iteration of this loop is a single step.

```c
void branch(struct config *conf, struct ncursesObjects *objects, struct counters *myCounters, int y, int x, enum branchType type, int life) {
	myCounters->branches++;
	int dx = 0;
	int dy = 0;
	int age = 0;
	int shootCooldown = conf->multiplier;

	while (life > 0) {
		if (checkKeyPress(conf, myCounters) == 1)
			quit(conf, objects, 0);

		life--;		// decrement remaining life counter
		age = conf->lifeStart - life;

		setDeltas(type, life, age, conf->multiplier, &dx, &dy);

		int maxY = getmaxy(objects->treeWin);
		if (dy > 0 && y > (maxY - 2)) dy--; // reduce dy if too close to the ground

		// near-dead branch should branch into a lot of leaves
		if (life < 3)
			branch(conf, objects, myCounters, y, x, dead, life);

		// dying trunk should branch into a lot of leaves
		else if (type == 0 && life < (conf->multiplier + 2))
			branch(conf, objects, myCounters, y, x, dying, life);

		// dying shoot should branch into a lot of leaves
		else if ((type == shootLeft || type == shootRight) && life < (conf->multiplier + 2))
			branch(conf, objects, myCounters, y, x, dying, life);

		else if (type == trunk && (((rand() % 3) == 0) || (life % conf->multiplier == 0))) {

			// if trunk is branching and not about to die, create another trunk with random life
			if ((rand() % 8 == 0) && life > 7) {
				shootCooldown = conf->multiplier * 2;	// reset shoot cooldown
				branch(conf, objects, myCounters, y, x, trunk, life + (rand() % 5 - 2));
			}

			// otherwise create a shoot
			else if (shootCooldown <= 0) {
				shootCooldown = conf->multiplier * 2;	// reset shoot cooldown

				int shootLife = (life + conf->multiplier);

				// first shoot is randomly directed
				myCounters->shoots++;
				myCounters->shootCounter++;
				if (conf->verbosity) mvwprintw(objects->treeWin, 4, 5, "shoots: %02d", myCounters->shoots);

				// create shoot
				branch(conf, objects, myCounters, y, x, (myCounters->shootCounter % 2) + 1, shootLife);
			}
		}
		shootCooldown--;

		// move in x and y directions
		x += dx;
		y += dy;

		chooseColor(type, objects->treeWin);

		// choose string to use for this branch
		char *branchStr = chooseString(conf, type, life, dx, dy);

		mvwprintw(objects->treeWin, y, x, "%s", branchStr);

		free(branchStr);

		// if live, update screen
		if (conf->live)
			updateScreen(conf->timeStep);
	}
}
```

### `setDeltas()`

This function determines the direction the branch will grow in.

```c
void setDeltas(enum branchType type, int life, int age, int multiplier, int *returnDx, int *returnDy) {
	// ... (implementation from ref.c)
}
```

### `chooseString()`

This function selects the characters to draw for a piece of the tree.

```c
char* chooseString(const struct config *conf, enum branchType type, int life, int dx, int dy) {
	// ... (implementation from ref.c)
}
```

### `updateScreen()`

This function is called in live mode to render the changes and pause.

```c
void updateScreen(float timeStep) {
	update_panels();
	doupdate();

	// convert given time into seconds and nanoseconds and sleep
	struct timespec ts;
	ts.tv_sec = timeStep / 1;
	ts.tv_nsec = (timeStep - ts.tv_sec) * 1000000000;
	nanosleep(&ts, NULL);	// sleep for given time
}
```
