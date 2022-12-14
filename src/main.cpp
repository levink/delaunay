#include <iostream>
#include "ui/ui.h"
#include "render.h"
#include "model/color.h"

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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Delaunay", nullptr, nullptr);
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

    float x1 = 50;
    float x2 = 500;
    float y1 = 50; 
    float y2 = 500;
    float radius = 6.0;

    std::vector<CircleVertex> vertex {
        CircleVertex{ x1, y1, -1, -1, radius, 1.f },
        CircleVertex{ x1, y1, +1, -1, radius, 1.f },
        CircleVertex{ x1, y1, +1, +1, radius, 1.f },
        CircleVertex{ x1, y1, -1, +1, radius, 1.f },

        CircleVertex{ x1, y2, -1, -1, radius, 1.f },
        CircleVertex{ x1, y2, +1, -1, radius, 1.f },
        CircleVertex{ x1, y2, +1, +1, radius, 1.f },
        CircleVertex{ x1, y2, -1, +1, radius, 1.f },
    
        CircleVertex{ x2, y2, -1, -1, radius, 1.f },
        CircleVertex{ x2, y2, +1, -1, radius, 1.f },
        CircleVertex{ x2, y2, +1, +1, radius, 1.f },
        CircleVertex{ x2, y2, -1, +1, radius, 1.f },
    };
    std::vector<Face> face {
        Face(0, 1, 2),
        Face(2, 3, 0),
        Face(4, 5, 6),
        Face(6, 7, 4),
        Face(8, 9, 10),
        Face(10, 11, 8)
    };

    CircleMesh batch;
    batch.color = Color::orange;
    batch.vertex = vertex;
    batch.face = face;

    GLuint vbo = 0;
    GLuint ibo = 0;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ARRAY_BUFFER, batch.vertex.size() * sizeof(CircleVertex), batch.vertex.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch.face.size() * sizeof(Face), batch.face.data(), GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   
    while (!glfwWindowShouldClose(window)) {
        render.draw();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        render.shaders.circle.enable();
        render.shaders.circle.drawVBO(batch);
        render.shaders.circle.disable();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    vbo = 0;
    ibo = 0;


    render.destroy();
    glfwTerminate();
    return 0;
}
