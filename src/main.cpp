#include <iostream>
#include <random>
#include <src/model/color.h>
#include "render.h"
#include "ui.h"

Render render;

void reshape(GLFWwindow*, int w, int h) {
    render.reshape(w, h);
}
void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    using namespace ui::keyboard;
    auto keyEvent = KeyEvent(key, action, mods);

    if (keyEvent.is(ESC)) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (keyEvent.is(R)) {
        std::cout << "Reload shaders" << std::endl;
        Platform platform;
        render.reloadShaders(platform);
    }
}
void mouseCallback(ui::mouse::MouseEvent mouseEvent) {
    using namespace ui::mouse;
    if (mouseEvent.is(Action::PRESS, Button::LEFT)) {
        auto cursor = mouseEvent.getCursor();
        auto x = cursor.x;
        auto y = (float)render.camera.viewSize.y - cursor.y;

        auto circle = CircleModel(x, y, 3.0, true);
        render.add(circle);

        auto vertex = LineVertex(x, y);
        render.lineVertices.push_back(vertex);
    }
}
void mouseClick(GLFWwindow*, int button, int action, int mods) {
    auto event = ui::mouse::MouseEvent(button, action, mods);
    mouseCallback(event);
}
void mouseMove(GLFWwindow*, double x, double y) {
    auto event = ui::mouse::MouseEvent(x, y);
    mouseCallback(event);
}
int main() {

    if (!glfwInit()) {
        return -1;
    }

    const auto WIDTH = 800;
    const auto HEIGHT = 600;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Delone", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to init glad" << std::endl;
        return -1;
    } else {
        printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
        printf("GLSL_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    glfwSetWindowPos(window, 950, 50);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseClick);
    glfwSetCursorPosCallback(window, mouseMove);
    glfwSwapInterval(1);
    glfwSetTime(0.0);

    Platform platform;
    render.load(platform);
    render.init();
    render.reshape(WIDTH, HEIGHT);

    auto& viewSize = render.camera.viewSize;
    std::random_device rd;
    std::mt19937 mt(rd());
    for(int i=0; i < 10; i++) {

        std::uniform_real_distribution<float> getRandomX(0.f, (float)viewSize.x);
        std::uniform_real_distribution<float> getRandomY(0.f, (float)viewSize.y);
        std::uniform_real_distribution<float> getRandomRadius(50.f, 120.f);

        float x = getRandomX(mt);
        float y = getRandomY(mt);
        float r = getRandomRadius(mt);
        bool fill = i % 3;
        render.add(CircleModel(x, y, r, fill));
    }

    while (!glfwWindowShouldClose(window)) {
        render.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    render.destroy();
    glfwTerminate();
    return 0;
}
