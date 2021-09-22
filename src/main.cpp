#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "ui.h"

Render render;

void reshape(GLFWwindow*, int width, int height) {

}
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    using namespace ui::keyboard;
    auto keyEvent = KeyEvent(key, action, mods);
    if (keyEvent.is(ESC))
        glfwSetWindowShouldClose(window, GL_TRUE);
}
void mouseClick(GLFWwindow*, int button, int action, int mods) {
    using namespace ui::mouse;
    auto mouseEvent = MouseEvent(button, action, mods);
    if (mouseEvent.is(Action::PRESS, Button::LEFT)) {
        auto cursor = mouseEvent.getCursor();
        std::cout << cursor.x << " " << cursor.y << std::endl;
    }
}
void mouseMove(GLFWwindow*, double x, double y) {
    using namespace ui::mouse;
    auto mouseEvent = MouseEvent(x, y);
}
int main() {

    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 860, "Delone", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to init glad" << std::endl;
        return -1;
    } else {
        printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
    }

    glfwSetWindowPos(window, 600, 75);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseClick);
    glfwSetCursorPosCallback(window, mouseMove);
    glfwSwapInterval(1);
    glfwSetTime(0.0);

    Platform master;
    render.load(master);
    render.init();

    while (!glfwWindowShouldClose(window)) {
        render.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    render.destroy();
    glfwTerminate();
    return 0;
}
