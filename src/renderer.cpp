#include "renderer.hpp"
#include "bvh.cpp"

// std
#include <iostream>
#include <fstream>
#include <execution>
#include <omp.h>

glm::vec3 Renderer::convertToRGB(const glm::vec3 &color)
{
    float scale = 1.f / (float)m_Camera->getSampleCount();
    auto r = color.r * scale * 255.f;
    auto g = color.g * scale * 255.f;
    auto b = color.b * scale * 255.f;

    return {(uint32_t)r, (uint32_t)g, (uint32_t)b};
}

Renderer::Renderer(Camera &camera, Scene &scene)
{
    m_Camera = &camera;
    m_Scene = &scene;
    m_Scene->getBVHTreeRoot();
    setup();
}

void Renderer::render()
{
    float aspectRatio = (float)m_ImageWidth / (float)m_ImageHeight;
    uint32_t sampleCount = m_Camera->getSampleCount();
#pragma omp parallel
    for (uint32_t y = 0; y < m_ImageHeight; ++y)
    {
#pragma omp parallel
        for (uint32_t x = 0; x < m_ImageWidth; ++x)
        {
            glm::vec3 color{};
            for (uint32_t spp = 0; spp < sampleCount; ++spp)
            {
                color += pixelResult(x, y);
            }
            m_PixelData[x + y * m_ImageWidth] = convertToRGB(color);
        }
    }
    generateImage();
}

void Renderer::setup()
{
    m_ImageWidth = m_Camera->getWidth();
    m_ImageHeight = m_Camera->getHeight();
    m_PixelData.resize(m_ImageHeight * m_ImageWidth);
}

void Renderer::generateImage()
{
    std::ofstream outfile("output.ppm");
    outfile << "P3\n"
            << m_ImageWidth << ' ' << m_ImageHeight << "\n255\n";

    for (uint32_t y = 0; y < m_ImageHeight; y++)
    {
        for (uint32_t x = 0; x < m_ImageWidth; x++)
        {
            outfile << (int)m_PixelData[x + y * m_ImageWidth].x << ' '
                    << (int)m_PixelData[x + y * m_ImageWidth].y << ' '
                    << (int)m_PixelData[x + y * m_ImageWidth].z << '\n';
        }
    }
    outfile.close();
    std::cout << "Image generated successfully!" << std::endl;
}

glm::vec3 Renderer::pixelResult(uint32_t x, uint32_t y)
{
    Ray ray = m_Camera->getRay(x, y);

    glm::vec3 light = m_Camera->getPosition();

    auto &meshes = m_Scene->getGeometries();

    HitResult hitResult;
    auto node = m_Scene->getBVHTreeRoot();
    float hitTime = std::numeric_limits<float>::max();
    if (intersectRayBVH(ray, node, hitResult, hitTime))
    {
        float intensity = glm::dot(glm::normalize(hitResult.hitWorldNormal), glm::normalize(light - hitResult.hitWorldPosition));
        return glm::vec3(1, 0, 1) * std::max(0.f, intensity);
    }
    return {0, 0, 0};
}
