**Project: Game Codebase Enhancement**
**Start Date:** May 29, 2025

---

### Phase 1: Core Rendering & Shader Abstraction (Deadline: June 7, 2025)

- [x] **Task 1.1:** Create a `Shader` class.
  - Responsibilities: Load shader source from files, compile vertex and fragment shaders, link them into a shader program, provide methods to use/unuse the program, and set uniforms (though uniforms aren't used yet).
  - Move existing shader source strings from Game.cpp into separate files (e.g., `triangle.vert`, `triangle.frag`).
- [x] **Task 1.2:** Refactor `Game::loadShaders` to use the new `Shader` class.
  - The `Game` class should now have a `Shader` member object (or a smart pointer to one).
- [ ] **Task 1.3:** Create a `Mesh` class (or struct).
  - Responsibilities: Encapsulate VAO, VBO, EBO (if using index drawing later), vertex data, and vertex attribute configuration.
  - Methods: `setupMesh(vertices, indices)`, `draw()`.
- [ ] **Task 1.4:** Refactor `Game::setupGameObjects` to create an instance of your `Mesh` class for the triangle.
  - The `Game` class will hold this `Mesh` object (or a collection if you plan for more simple objects soon).
- [ ] **Task 1.5:** Update `Game::render` to use the `Shader` class's `use()` method and call the `Mesh` class's `draw()` method.

---

### Phase 2: Basic Game Object Representation (Deadline: June 14, 2025)

- [ ] **Task 2.1:** Design and implement a simple `GameObject` class.
  - Initial properties: Could hold a pointer/reference to a `Mesh`, and potentially basic transform data (position, rotation, scale - even if not fully utilized yet).
  - Methods: `update(deltaTime)`, `render()`.
- [ ] **Task 2.2:** Modify `Game` class to manage a list/vector of `GameObject`s.
- [ ] **Task 2.3:** Update `Game::setupGameObjects` to create `GameObject` instances (e.g., one for the triangle, associating it with the `Mesh` created in Phase 1).
- [ ] **Task 2.4:** Update `Game::update` and `Game::render` to iterate through the `GameObject`s and call their respective methods.

---

### Phase 3: Input Handling Abstraction (Deadline: June 21, 2025)

- [ ] **Task 3.1:** Design and implement an `InputManager` class.
  - Responsibilities: Abstract away direct GLFW calls for key states.
  - Methods: `isKeyPressed(keyCode)`, `isKeyReleased(keyCode)`, `isMouseButtonPressed(button)`, etc.
  - (Optional advanced): Map raw inputs to game actions (e.g., "Jump", "MoveForward").
- [ ] **Task 3.2:** Integrate `InputManager` into the `Game` class.
  - `Game::processInput` should now query the `InputManager`.

---

### Phase 4: Basic Resource Management (Focus on Shaders & Meshes) (Deadline: June 28, 2025)

- [ ] **Task 4.1:** Design a `ResourceManager` (template or specific managers like `ShaderManager`, `MeshManager`).
  - Responsibilities: Load, store, and provide access to resources like Shaders and Meshes. Prevent duplicate loading.
  - Methods: `loadShader(name, vertPath, fragPath)`, `getShader(name)`, `loadMesh(...)`, `getMesh(...)`.
- [ ] **Task 4.2:** Refactor `Game` (and potentially `GameObject`) to request Shaders and Meshes from the `ResourceManager` instead of creating them directly.

---

### Phase 5: Game State Management (Initial Setup) (Deadline: July 5, 2025)

- [ ] **Task 5.1:** Define an abstract `IGameState` base class or interface.
  - Virtual methods: `init()`, `handleInput(InputManager&)`, `update(float deltaTime)`, `render()`, `cleanup()`.
- [ ] **Task 5.2:** Create a concrete `PlayingState` class derived from `IGameState`.
  - Move the current core game logic (object creation, input processing for gameplay, updating, rendering) from `Game` into `PlayingState`.
- [ ] **Task 5.3:** Implement a `GameStateManager` class.
  - Responsibilities: Manage a stack or list of game states, handle transitions (`pushState`, `popState`, `changeState`).
  - The `Game` class will own an instance of `GameStateManager`.
- [ ] **Task 5.4:** Refactor `Game::run` loop to delegate `handleInput`, `update`, and `render` calls to the current state managed by `GameStateManager`.
  - `Game::init` would likely initialize and push the initial `PlayingState`.

---
