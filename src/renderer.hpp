#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "ray.hpp"
#include "camera.hpp"
#include "scene.hpp"

// thirdparty
#include <glm/glm.hpp>

// std
#include <vector>
#include <memory>

class Renderer
{
public:
    Renderer() = default;
    Renderer(Camera &camera, Scene &scene);
    void render();

    void setBoudingTime(int bt) { boundTime = bt; }

private:
    void setup();
    void generateImage();
    glm::vec3 convertToRGB(const glm::vec3 &color);
    glm::vec3 pixelResult(uint32_t x, uint32_t y);
    glm::vec3 traceRay(const Ray &ray, int depth);

private:
    uint32_t m_ImageWidth, m_ImageHeight;
    std::vector<glm::vec3> m_PixelData{};
    uint32_t boundTime = 5;

    Camera *m_Camera = nullptr;
    Scene *m_Scene = nullptr;
};
#endif // __RENDERER_H__