#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <glm/glm.hpp>
#include "ray.hpp"
#include "camera.hpp"
#include "scene.hpp"

// std
#include <vector>
#include <memory>

class Renderer
{
public:
    Renderer() = default;
    void render(Camera &camera, Scene &scene);

private:
    void setup();
    void generateImage();
    glm::vec3 convertToRGB(const glm::vec3 &color);
    glm::vec3 pixelResult(uint32_t x, uint32_t y);

private:
    uint32_t m_ImageWidth, m_ImageHeight;
    std::vector<glm::vec3> m_PixelData{};

    Camera *m_Camera = nullptr;
    Scene *m_Scene = nullptr;
};
#endif // __RENDERER_H__