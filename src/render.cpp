#include <random>
#include "src/model/color.h"
#include "render.h"


void ShaderCache::load(const ShaderLoader &loader) {
    circle = loader.load(files::circle);
    line = loader.load(files::line);
}
void Shaders::create(const ShaderCache &cache) {
    circle.create(cache.circle);
    line.create(cache.line);
}
void Shaders::link(const Render &render) {
    circle.link(&render.camera);
    line.link(&render.camera);
}
void Shaders::destroy() {
    circle.destroy();
    line.destroy();
}
void Render::loadResources(Platform &platform) {
    if (shaderCache.loaded) {
        return;
    }
    ShaderLoader loader(platform);
    shaderCache.load(loader);
}
void Render::initResources() {
    shaders.create(shaderCache);
    shaders.link(*this);
}
void Render::destroy() {
    shaders.destroy();
}
void Render::initScene() {
    //layer
    {

        scene.initScene(camera.viewSize);

    }

#if 0 //circles batch
    {
        std::random_device rd;
        std::mt19937 mt(rd());

        CircleBatch batch;
        auto& vertex = batch.vertex;
        auto& face = batch.face;

        int count = 20;
        vertex.reserve(4 * count);
        face.reserve(2 * count);

        for(int i = 0; i < count; i++) {

            std::uniform_real_distribution<float> getRandomX(0.f, (float)camera.viewSize.x);
            std::uniform_real_distribution<float> getRandomY(0.f, (float)camera.viewSize.y);
            std::uniform_real_distribution<float> getRandomRadius(50.f, 120.f);

            float x = getRandomX(mt);
            float y = getRandomY(mt);
            float r = getRandomRadius(mt);
            float fill = 0.f;//(i % 7) ? 0.f : 1.f;

            int offset= i * 4;
            vertex.emplace_back(x - r, y - r, x, y, r, fill);
            vertex.emplace_back(x - r, y + r, x, y, r, fill);
            vertex.emplace_back(x + r, y + r, x, y, r, fill);
            vertex.emplace_back(x + r, y - r, x, y, r, fill);

            face.emplace_back(offset + 0,offset + 1,offset + 2);
            face.emplace_back(offset + 2,offset + 3,offset + 0);
        }

        GLuint vbo;
        auto vertexData = vertex.data();
        auto vertexSize = (long)vertex.size() * (long)sizeof(CircleVertex);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);

        GLuint ibo;
        auto faceData = face.data();
        auto faceSize = (long)face.size() * (long)sizeof(Face);
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceSize, faceData, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        circlesBatch.vbo = vbo;
        circlesBatch.ibo = ibo;
        circlesBatch.count = batch.face.size() * 3;
    }
#endif
}
void Render::draw() {

    auto background = glm::vec4(Color::asphalt, 1.0);
    glClearColor(background.r,
                 background.g,
                 background.b,
                 background.a);
    glClear(GL_COLOR_BUFFER_BIT);

    shaders.line.enable();
    shaders.line.draw(scene.trianglesMesh);
    shaders.line.disable();

    shaders.circle.enable();
    shaders.circle.draw(scene.getSelectedCircle());
    shaders.circle.draw(scene.pointsMesh);
    shaders.circle.disable();
}
void Render::reshape(int w, int h) {
    camera.reshape(w, h);
}
void Render::reloadShaders(Platform &platform) {
    ShaderLoader loader(platform);
    shaderCache.load(loader);
    shaders.destroy();
    shaders.create(shaderCache);
    shaders.link(*this);
}
