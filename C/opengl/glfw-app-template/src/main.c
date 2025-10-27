#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

static const float FIXED_TIMESTEP = 1.0F / 20.0F;
static const float MAX_FRAME_TIME = 0.25F;

struct AppContext {
	int width, height;
	struct {
		double now, last;
		double accumulator;
		float frameDelta;
		float tickDelta;
		uint64_t tick;
	} time;
};

static void glfw_error_callback(int error_code, const char *description);
static void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void input(GLFWwindow *window);
static void update(GLFWwindow *window);
static void render(GLFWwindow *window);

int main(void) {
	struct AppContext ctx = {
		.width = 500,
		.height = 500,
	};

	glfwSetErrorCallback(glfw_error_callback);

#ifdef DEBUG
	int major, minor, revision = 0;
	glfwGetVersion(&major, &minor, &revision);
	printf("Initializing GLFW v%i.%i.%i\n", major, minor, revision);
#endif

	if (GLFW_FALSE == glfwInit()) {
		return EXIT_FAILURE;
	}

	// Use 4.1 core profile due to macOS support
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *window = glfwCreateWindow(ctx.width, ctx.height, "Hello, GLFW world!", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetWindowUserPointer(window, &ctx);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);

	glfwMakeContextCurrent(window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		fprintf(stderr, "Failed to load OpenGL\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	int fbw, fbh;
	glfwGetFramebufferSize(window, &fbw, &fbh);
	glViewport(0, 0, fbw, fbh);

#ifdef DEBUG
	printf("Loaded OpenGL v%d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
#endif

	glfwSwapInterval(1); // VSync by default
	glClearColor(135.0F / 255.0F, 206.0F / 255.0F, 235.0F / 255.0F, 1.0F);

	ctx.time.now = glfwGetTime();
	ctx.time.last = ctx.time.now;
	ctx.time.accumulator = 0.0;
	ctx.time.frameDelta = 0.0F;
	ctx.time.tickDelta = 0.0F;
	ctx.time.tick = 0;

	while (!glfwWindowShouldClose(window)) {
		// ---- Timing -----
		ctx.time.now = glfwGetTime();
		ctx.time.frameDelta = (float)(ctx.time.now - ctx.time.last);
		if (ctx.time.frameDelta > MAX_FRAME_TIME) ctx.time.frameDelta = MAX_FRAME_TIME;
		ctx.time.accumulator += ctx.time.frameDelta;
		ctx.time.last = ctx.time.now;

		// ---- Input -----
		glfwPollEvents();
		input(window);

		// ---- Update -----
		while (ctx.time.accumulator >= FIXED_TIMESTEP) {
			update(window);
			ctx.time.accumulator -= FIXED_TIMESTEP;
			ctx.time.tick++;
		}

		ctx.time.tickDelta = (float)(ctx.time.accumulator / FIXED_TIMESTEP);
		if (ctx.time.tickDelta > 1.0F) ctx.time.tickDelta = 1.0F;

		// ---- Render -----
		glClear(GL_COLOR_BUFFER_BIT);
		render(window);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}

static void glfw_error_callback(int error_code, const char *description) {
	fprintf(stderr, "GLFW Error(%d): %s\n", error_code, description);
}

static void glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);

	struct AppContext *ctx = glfwGetWindowUserPointer(window);
	if (ctx != NULL) {
		ctx->width = width;
		ctx->height = height;
	}
}

static void input(GLFWwindow *window) {
	static bool wireframe = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		wireframe = !wireframe;
	}
}

static void update(GLFWwindow *window) {
	struct AppContext *ctx = glfwGetWindowUserPointer(window);
	assert(ctx != NULL);

	static double lastLog = 0.0;
	static int ticksSinceLog = 0;

	ticksSinceLog++;
	double now = ctx->time.now;
	if (now - lastLog >= 1.0) {
		printf("[TICK] %llu ticks | TPS: %d\n", ctx->time.tick, ticksSinceLog);
		ticksSinceLog = 0;
		lastLog = now;
	}
}

static void render(GLFWwindow *window) {
	struct AppContext *ctx = glfwGetWindowUserPointer(window);
	assert(ctx != NULL);

	static double lastLog = 0.0;
	static int framesSinceLog = 0;

	framesSinceLog++;
	double now = ctx->time.now;
	if (now - lastLog >= 1.0) {
		printf("[RENDER] FPS: %d | tickDelta: %.2f\n", framesSinceLog, ctx->time.tickDelta);
		framesSinceLog = 0;
		lastLog = now;
	}
}
