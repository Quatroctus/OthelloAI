#include <iostream>
#include <chrono>

#include "game.h"
#include "renderer.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glfw3/glfw3.h>

using Clock = std::chrono::high_resolution_clock;

bool Running = true;

#ifdef WIN32
#define _SLEEP(x) _sleep(x)
#else
#include <unistd.h>
#define _SLEEP(x) usleep(x)
#endif

void SetupGLFWEventCallbacks(GLFWwindow* window) {
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        Running = false;
    });

}

int main(int argc, char** args) {
    if (argc < 3) {
        std::cerr << "Usage: " << args[0] << " <player_type> <player_type>" << std::endl;
        return 2;
    }

    PlayerType player1Type = GetPlayerType(args[1]);
    PlayerType player2Type = GetPlayerType(args[2]);

    if (player1Type == PlayerType::NONE) {
        std::cerr << "Invalid player_type: " << args[1] << ".\n    Valid player_types are: human and minimax." << std::endl;
        return 2;
    }
    if (player2Type == PlayerType::NONE) {
        std::cerr << "Invalid player_type: " << args[2] << ".\n    Valid player_types are: human and minimax." << std::endl;
        return 2;
    }

    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "GLFW failed to initialize. Likely no graphics device found.\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =  glfwCreateWindow(720, 480, "Othello", NULL, NULL);
    assert(window);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        exit(1);
    }
    glfwSwapInterval(0);
    SetupGLFWEventCallbacks(window);

    RendererInit();

    glClearColor(.22f, .22f, .22f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    constexpr double FRAME_TARGET = 1.0 / 60.0;
    constexpr double NANOSECONDS_PER_SECOND = 1000000000;
    auto lastTime = Clock::now(), currentTime = lastTime;
    double elapsedTime = FRAME_TARGET;
    double totalElapsedTime = 0.0;
    int frames = 0;
    while (Running) {
        glfwPollEvents();
        currentTime = Clock::now();
        double temp = (currentTime - lastTime).count() / NANOSECONDS_PER_SECOND;
        elapsedTime += temp;
        totalElapsedTime += temp;
        lastTime = currentTime;
        if (totalElapsedTime > 1.0) {
            std::cout << "Frames: " << frames << std::endl;
            totalElapsedTime -= 1.0;
            frames = 0;
        }
        if (elapsedTime > FRAME_TARGET) {
            frames++;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Run the othello board / AI
            RenderBoard();
            RendererFlush();
            glfwSwapBuffers(window);
            glFlush();
            elapsedTime -= FRAME_TARGET;
        } else {
            _SLEEP(1);
        }
    }
    RendererShutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
