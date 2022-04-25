#include <iostream>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glfw3/glfw3.h>

int main(int argc, char** args) {
    std::cout << "Hello World" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =  glfwCreateWindow(720, 480, "Othello", NULL, NULL);
    assert(window);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glm::vec3 test{0.0f};
    std::cout << test.x << ", " << test.y << ", " << test.z << ", " << std::endl;

    while (true);

    return 0;
}
