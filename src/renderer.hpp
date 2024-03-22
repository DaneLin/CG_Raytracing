#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <glm/glm.hpp>
#include "ray.hpp"
#include "camera.hpp"

// std
#include <vector>
#include <memory>

class Renderer
{
public:
    Renderer() = default;
    void render(const Camera &camera);

private:
    void setup();
    void generateImage();
    glm::vec3 pixelResult(uint32_t x, uint32_t y);

private:
    uint32_t m_ImageWidth, m_ImageHeight;
    std::vector<glm::vec3> m_PixelData{};

    const Camera *m_Camera = nullptr;
};
#endif // __RENDERER_H__