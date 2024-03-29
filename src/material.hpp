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

    virtual glm::vec3 emit() const
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
    Lambertian(glm::vec3 color) : albedo(color) {}
    Lambertian(std::shared_ptr<Texture> &texture) : texture(texture) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, float &pdf) const override
    {
        glm::vec3 scatterDirection = arc::randomInHemisphere(hitResult.hitWorldNormal);

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
        float cosine = glm::dot(hitResult.hitWorldNormal, rayOut.direction);
        return cosine < 0 ? 0 : cosine / PI;
    }

public:
    glm::vec3 albedo{};
    std::shared_ptr<Texture> texture = nullptr;
};

class LambertianTextured : public Material
{
public:
    LambertianTextured(std::shared_ptr<Texture> tex) : texture(tex) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, float &pdf) const override
    {
        glm::vec3 scatterDirection = arc::randomInHemisphere(hitResult.hitWorldNormal);

        if (arc::isNearZero(scatterDirection))
            scatterDirection = hitResult.hitWorldNormal;
        scatterResult.attenuation = texture->sample(hitResult.hitUV.x, hitResult.hitUV.y);
        scatterResult.rayOut = Ray(hitResult.hitWorldPosition, scatterDirection);
        scatterResult.skipPDF = false;
        pdf = glm::dot(hitResult.hitWorldNormal, scatterDirection) / PI;
        return true;
    }

    float scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const override
    {
        float cosine = glm::dot(hitResult.hitWorldNormal, rayOut.direction);
        return cosine < 0 ? 0 : cosine / PI;
    }

public:
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

    glm::vec3 emit() const override
    {
        return radiance;
    }

private:
    // light energy
    glm::vec3 radiance{};
};

class GlossyMaterial : public Material
{
public:
    GlossyMaterial(const glm::vec3 &albedo, float roughness)
        : m_Albedo(albedo), m_Roughness(roughness) {}

    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatteredResult, float &pdf) const override
    {
        glm::vec3 halfway = glm::normalize(ray.direction + hitResult.hitWorldNormal);
        float nDotH = std::max(0.0f, glm::dot(hitResult.hitWorldNormal, halfway));

        // Cook-Torrance microfacet BRDF
        float F = fresnel(glm::dot(ray.direction, halfway), 0.04);
        float G = smithGGXCorrelated(glm::dot(hitResult.hitWorldNormal, ray.direction), glm::dot(hitResult.hitWorldNormal, glm::normalize(ray.direction)), m_Roughness);
        float D = GGX(nDotH, m_Roughness);

        float brdf = (F * G * D) / (4.0f * glm::dot(hitResult.hitWorldNormal, ray.direction) * glm::dot(hitResult.hitWorldNormal, glm::normalize(ray.direction)));

        glm::vec3 reflectDir = glm::reflect(-ray.direction, hitResult.hitWorldNormal);
        scatteredResult.rayOut = Ray(hitResult.hitWorldPosition, reflectDir);
        scatteredResult.attenuation = m_Albedo * brdf;
        scatteredResult.skipPDF = false;
        pdf = brdf * glm::dot(hitResult.hitWorldNormal, glm::normalize(scatteredResult.rayOut.direction)) / PI;
        return true;
    }

    float scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const override
    {
        glm::vec3 halfway = glm::normalize(rayIn.direction + rayOut.direction);
        float nDotH = std::max(0.0f, glm::dot(hitResult.hitWorldNormal, halfway));
        float brdf = GGX(nDotH, m_Roughness);
        return brdf * glm::dot(hitResult.hitWorldNormal, glm::normalize(rayOut.direction)) / PI;
    }

private:
    glm::vec3 m_Albedo;
    float m_Roughness;

    static float fresnel(float cosTheta, float f0)
    {
        return f0 + (1.0f - f0) * std::pow(1.0f - cosTheta, 5.0f);
    }

    static float GGX(float nDotH, float roughness)
    {
        float a = roughness * roughness;
        float a2 = a * a;
        float nDotH2 = nDotH * nDotH;
        return a2 / (PI * (nDotH2 * (a2 - 1.0f) + 1.0f) * (nDotH2 * (a2 - 1.0f) + 1.0f));
    }

    static float smithGGXCorrelated(float nDotV, float nDotL, float roughness)
    {
        float a = roughness * roughness;
        float nDotV2 = nDotV * nDotV;
        float nDotL2 = nDotL * nDotL;
        return 0.5f / (nDotV + std::sqrt(a + nDotV2 * (1.0f - a))) * 0.5f / (nDotL + std::sqrt(a + nDotL2 * (1.0f - a)));
    }
};

#endif