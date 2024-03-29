#include "renderer.hpp"
#include "bvh.cpp"
#include "helper.hpp"
#include "record.hpp"

// std
#include <iostream>
#include <fstream>
#include <execution>
#include <omp.h>
#include <iomanip>

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
    // #pragma omp parallel
    for (uint32_t y = 0; y < m_ImageHeight; ++y)
    {
        std::clog << "\rCreating image: " << std::fixed << std::setprecision(2) << ((float)y / (float)m_ImageHeight) * 100.f << "%" << std::flush;
        int x = 0;
#pragma omp parallel 
        for( x = 0; x < m_ImageWidth; ++x)
        {
            int idx = x + y * m_ImageWidth;
            glm::vec3 color{};
            for (uint32_t spp = 0; spp < sampleCount; ++spp)
            {
                color += pixelResult(x, y);
            }
            m_PixelData[idx] = convertToRGB(color);
        }
    }
    generateImage();
}

void Renderer::setup()
{
    m_ImageWidth = m_Camera->getWidth();
    m_ImageHeight = m_Camera->getHeight();
    m_PixelData.resize(m_ImageHeight * m_ImageWidth);
    node = m_Scene->getBVHTreeRoot();
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
    std::clog << "\rDone.                  \n";
}

glm::vec3 Renderer::pixelResult(uint32_t x, uint32_t y)
{
    Ray ray = m_Camera->getRay(x, y);
    return traceRay(ray, boundTime);
}

glm::vec3 Renderer::traceRay(const Ray &ray, int depth)
{
    // depth <=0, we stop ray color collection
    if (depth <= 0)
        return glm::vec3(0.f);

    float hitTime = std::numeric_limits<float>::max();
    HitResult hitResult = intersectRayBVH(ray, node);
    if (!hitResult.isHit)
    {
        return glm::vec3(0.f);
    }
    auto currentMat = hitResult.mat;
    auto emissive = currentMat->emit();
    if (emissive != glm::vec3(0.f))
        return emissive;

    glm::vec3 directLight{0.f}, indirectLight{0.f};
    // direct light
    //float pdfLight = 0.f;
    //int lightIdx = arc::randomInt(m_Scene->getLights().size());
    //auto light = m_Scene->getLights();
    //HitResult lightHitResult;
    //light[lightIdx].sample(lightHitResult, pdfLight);
    //glm::vec3 objPoint = hitResult.hitWorldPosition;
    //glm::vec3 lightPoint = lightHitResult.hitWorldPosition;
    //glm::vec3 obj2Light = lightPoint - objPoint;
    //float distance = glm::dot(obj2Light, obj2Light);
    //obj2Light = glm::normalize(obj2Light);

    //glm::vec3 objNormal = hitResult.hitWorldNormal;
    //glm::vec3 lightNormal = lightHitResult.hitWorldNormal;

    //Ray toLightRay(hitResult.hitWorldPosition, obj2Light);
    //HitResult toLight = intersectRayBVH(toLightRay, node);
    //if (toLight.distance - distance > -0.0000001f)
    //{
    //    directLight = light[lightIdx].mat->emit(); //* glm::dot(toLightRay.direction, objNormal) * glm::dot(-toLightRay.direction, lightNormal) / (distance);
    //}

    ScatteredResult scatteredResult;
    float pdf;
    if (!currentMat->scatter(ray, hitResult, scatteredResult, pdf))
    {
        return emissive;
    }
    auto scatterPDF = currentMat->scatterPDF(ray, hitResult, scatteredResult.rayOut);
    indirectLight = scatteredResult.attenuation  * traceRay(scatteredResult.rayOut, depth - 1);
    return indirectLight;
}

// glm::vec3 Renderer::traceRay(const Ray &ray, int depth)
// {
//     // depth <=0, we stop ray color collection
//     if (depth <= 0)
//         return glm::vec3(0.f);

//     HitResult hitResult;
//     auto node = m_Scene->getBVHTreeRoot();
//     float hitTime = std::numeric_limits<float>::max();
//     if (!intersectRayBVH(ray, node, hitResult, hitTime))
//     {
//         return glm::vec3(0.f);
//     }
//     ScatteredResult scatteredResult;
//     auto toHitPoint = hitResult.hitWorldPosition - ray.origin;
//     float distance = glm::dot(toHitPoint, toHitPoint);
//     auto emissive = m_Scene->getMaterials()[hitResult.faceMaterialID]->emit(ray, hitResult, hitResult.hitUV.x, hitResult.hitUV.y) / distance;
//     float pdf;
//     if (!m_Scene->getMaterials()[hitResult.faceMaterialID]->scatter(ray, hitResult, scatteredResult, pdf))
//     {
//         return emissive;
//     }
//     auto scatterPDF = m_Scene->getMaterials()[hitResult.faceMaterialID]->scatterPDF(ray, hitResult, scatteredResult.rayOut);
//     auto collection = scatteredResult.attenuation * scatterPDF * traceRay(scatteredResult.rayOut, depth - 1) / pdf;
//     return emissive + collection;
// }

// for (auto mesh : m_Scene->getGeometries())
// {
//     HitResult tmp = mesh.getIntersection(ray);
//     if (tmp.hitTime < hitResult.hitTime)
//         hitResult = tmp;
// }
// if (!hitResult.isHit)
//     return glm::vec3(0.f);