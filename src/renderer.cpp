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

void Renderer::render(Camera &camera, Scene &scene)
{
    m_Camera = &camera;
    m_Scene = &scene;
    m_Scene->getBVHTreeRoot();
    setup();
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
    Ray ray = m_Camera->getRay(x, y);

    glm::vec3 light = m_Camera->getPosition();
    // light = glm::normalize(light);

    // float radius = 0.5f;

    // float a = glm::dot(ray.direction, ray.direction);
    // float b = 2.f * glm::dot(ray.origin, ray.direction);
    // float c = glm::dot(ray.origin, ray.origin) - radius * radius;

    // float discriminant = b * b - 4.f * a * c;
    // if (discriminant > 0.0f)
    // {
    //     discriminant = std::sqrt(discriminant);
    //     float closestT = (-b - discriminant) / (2.f * a);

    //     glm::vec3 hitPoint = ray.origin + ray.direction * closestT;
    //     glm::vec3 hitNormal = glm::normalize(hitPoint);

    //     float intensity = std::max(0.f, glm::dot(hitNormal, light));
    //     return glm::vec3(1, 0, 1) * intensity;
    // }
    auto &meshes = m_Scene->getGeometries();

    HitResult hitResult;
    auto node = m_Scene->getBVHTreeRoot();
    float hitTime = std::numeric_limits<float>::max();
    if (intersectRayBVH(ray, node, hitResult, hitTime))
    {
        float intensity = glm::dot(glm::normalize(hitResult.hitWorldNormal), glm::normalize(light - hitResult.hitWorldPosition));
        return glm::vec3(1, 0, 1) * std::max(0.f, intensity);
    }
    // for (auto mesh : meshes)
    // {
    //     HitResult hitResult = mesh.getIntersection(ray);
    //     if (hitResult.isHit)
    //     {
    //         glm::vec3 hitPoint = ray.origin + ray.direction * hitResult.hitTime;
    //         glm::vec3 normal = hitResult.hitWorldNormal;
    //         if (glm::dot(normal, -light) < 0)
    //             normal = -normal;

    //         return glm::vec3(1, 0, 1) * std::max(0.f, glm::dot(glm::normalize(normal), -light));
    //     }
    // }

    return {0, 0, 0};
}
