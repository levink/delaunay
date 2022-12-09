#include <iostream>
#include "ui/ui.h"
#include "render.h"

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
void mouseCallback(ui::mouse::MouseEvent event) {
    using namespace ui;
    using namespace ui::mouse;

    auto cursor = event.getCursor();
    cursor.y = (float)render.camera.viewSize.y - cursor.y;
    auto& scene = render.scene;

    if (event.is(Action::PRESS, Button::LEFT)) scene.selectPoint(cursor);
    else if (event.is(Action::MOVE, Button::LEFT))  scene.movePoint(cursor);
    else if (event.is(Action::RELEASE, Button::LEFT)) scene.clearSelection();
    else if (event.is(Action::PRESS, Button::RIGHT)) scene.addPoint(cursor);
    else if (event.is(Action::PRESS, Button::RIGHT, KeyMod::ALT)) scene.deletePoint(cursor);
}
void mouseClick(GLFWwindow*, int button, int action, int mods) {
    auto event = ui::mouse::click(button, action, mods);
    mouseCallback(event);
}
void mouseMove(GLFWwindow*, double x, double y) {
    auto mx = static_cast<int>(x);
    auto my = static_cast<int>(y);
    auto event = ui::mouse::move(mx, my);
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
        std::cout << "Failed to initResources glad" << std::endl;
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
    render.loadResources(platform);
    render.initResources();
    render.reshape(WIDTH, HEIGHT);
    render.initScene();

    while (!glfwWindowShouldClose(window)) {
        render.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    render.destroy();
    glfwTerminate();
    return 0;
}
