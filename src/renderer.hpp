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
    glm::dvec3 convertToRGB(const glm::dvec3 &color);
    glm::dvec3 pixelResult(uint32_t x, uint32_t y);
    glm::dvec3 traceRay(const Ray &ray, int depth);

private:
    uint32_t m_ImageWidth, m_ImageHeight;
    std::vector<glm::dvec3> m_PixelData{};
    uint32_t boundTime = 5;
    const BVHNode *node = nullptr;

    Camera *m_Camera = nullptr;
    Scene *m_Scene = nullptr;
};
#endif // __RENDERER_H__