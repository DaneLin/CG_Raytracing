#include "renderer.hpp"

// std
#include <iostream>
#include <fstream>

glm::vec3 convertToRGB(const glm::vec3 &color)
{
    return {color.r * 255.f, color.g * 255.f, color.b * 255.f};
}

void Renderer::render(const Camera &camera)
{
    m_Camera = &camera;
    setup();
    float aspectRatio = (float)m_ImageWidth / (float)m_ImageHeight;
    for (uint32_t y = 0; y < m_ImageHeight; ++y)
    {
        for (uint32_t x = 0; x < m_ImageWidth; ++x)
        {
            m_PixelData[x + y * m_ImageWidth] = convertToRGB(pixelResult(x, y));
        }
    }
    generateImage();
}

// void Renderer::render()
// {
//     float aspectRatio = (float)m_ImageWidth / (float)m_ImageHeight;
//     for (uint32_t y = 0; y < m_ImageHeight; ++y)
//     {
//         for (uint32_t x = 0; x < m_ImageWidth; ++x)
//         {
//             glm::vec2 screenPos = {(float)x / (float)m_ImageWidth, (float)y / (float)m_ImageHeight};
//             screenPos = screenPos * 2.f - 1.f;
//             screenPos.x *= aspectRatio;

//             Ray ray({0.f, 0.f, 2.f}, {screenPos.x, screenPos.y, -1.f});
//             m_PixelData[x + y * m_ImageWidth] = convertToRGB(pixelResult(x, y));
//         }
//     }

//     generateImage();
// }

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
    Ray ray(m_Camera->getPosition(), m_Camera->getRayDirections()[x + y * m_ImageWidth]);

    glm::vec3 light{0.f, 1.f, 0.f};

    float radius = 0.5f;

    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.f * glm::dot(ray.origin, ray.direction);
    float c = glm::dot(ray.origin, ray.origin) - radius * radius;

    float discriminant = b * b - 4.f * a * c;
    if (discriminant > 0.0f)
    {
        discriminant = std::sqrt(discriminant);
        float closestT = (-b - discriminant) / (2.f * a);

        glm::vec3 hitPoint = ray.origin + ray.direction * closestT;
        glm::vec3 hitNormal = glm::normalize(hitPoint);

        float intensity = std::max(0.f, glm::dot(hitNormal, light));
        return glm::vec3(1, 0, 1) * intensity;
    }
    return {0, 0, 0};
}
