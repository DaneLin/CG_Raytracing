#ifndef __MATERIAL_H__
#define __MATERIAL_H__

// thirdparty
#include <glm/glm.hpp>

#include "ray.hpp"
#include "geometry.hpp"
#include "texture.hpp"
#include "helper.hpp"

#define PI 3.1415926

class Material
{
public:
    ~Material() = default;

    virtual bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, float &pdf) const
    {
        pdf = 0;
        return false;
    }

    virtual glm::vec3 emit(const Ray &ray, const HitResult &hitResult, double u, double v) const
    {
        return glm::vec3(0.f);
    }

    virtual float scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const
    {
        return 0.f;
    }
};

class Lambertian : public Material
{
public:
    Lambertian() = default;
    Lambertian(glm::vec3 color) : albedo(color) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, float &pdf) const override
    {
        glm::vec3 scatterDirection = hitResult.hitWorldNormal + arc::randomInUniformSphere();

        if (arc::isNearZero(scatterDirection))
            scatterDirection = hitResult.hitWorldNormal;
        scatterResult.attenuation = albedo;
        scatterResult.rayOut = Ray(hitResult.hitWorldPosition, scatterDirection);
        scatterResult.skipPDF = false;
        pdf = glm::dot(hitResult.hitWorldNormal, scatterDirection) / PI;
        return true;
    }

    float scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const override
    {
        float cosine = glm::dot(hitResult.hitWorldNormal, glm::normalize(rayOut.direction));
        return cosine < 0 ? 0 : cosine / PI;
    }

private:
    glm::vec3 albedo{};
    std::shared_ptr<Texture> texture = nullptr;
};

class Mirror : public Material
{
public:
    Mirror() = default;
    Mirror(const glm::vec3 &color) : albedo(color) {}

    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, float &pdf) const override
    {
        scatterResult.attenuation = albedo;
        scatterResult.rayOut.direction = glm::reflect(ray.origin, hitResult.hitWorldNormal);
        scatterResult.skipPDF = true;
        return true;
    }

private:
    glm::vec3 albedo{};
};

class LightSource : public Material
{
public:
    LightSource() = default;
    LightSource(const glm::vec3 &radiance) : radiance(radiance) {}

    glm::vec3 emit(const Ray &ray, const HitResult &hitResult, double u, double v) const override
    {
        return radiance;
    }

private:
    // light energy
    glm::vec3 radiance{};
};
#endif