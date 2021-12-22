
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "battle_system.hpp"
// #include <behaviour_tree.hpp>

using Clock = std::chrono::high_resolution_clock;

//const int window_width_px = 1200;
//const int window_height_px = 800; //* 2;

int frameBufferWidth;
int frameBufferHeight;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	BattleSystem battle;
    AISystem ai(&battle);

	// Initializing window
    GLFWwindow* window = world.create_window(window_width_px, window_height_px);
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glfwSetWindowAspectRatio(window, w, h);
    glfwSetWindowSizeLimits(window, window_width_px/2, window_height_px/2, window_width_px, window_height_px);
    if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}


    renderer.init(window_width_px, window_height_px, window);
    world.init(&renderer);
    battle.init(&world);
    world.attach(physics.getCollisionFunction());

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become
		// unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.step(elapsed_ms);
		ai.step(elapsed_ms);
		physics.step(elapsed_ms, window_width_px, window_height_px);
		world.handle_collisions();
		battle.handle_battle(elapsed_ms);
		renderer.draw();
	}

	return EXIT_SUCCESS;
}
