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

    virtual bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, double &pdf) const
    {
        pdf = 0;
        return false;
    }

    virtual glm::dvec3 emit() const
    {
        return glm::dvec3(0.f);
    }

    virtual double scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const
    {
        return 0.f;
    }
};

class Lambertian : public Material
{
public:
    Lambertian(glm::dvec3 color) : albedo(color) {}
    Lambertian(std::shared_ptr<Texture> &texture) : texture(texture) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, double &pdf) const override
    {
        glm::dvec3 scatterDirection = arc::randomInHemisphere(hitResult.hitWorldNormal);

        if (arc::isNearZero(scatterDirection))
            scatterDirection = hitResult.hitWorldNormal;
        scatterResult.attenuation = albedo;
        scatterResult.rayOut = Ray(hitResult.hitWorldPosition, scatterDirection);
        scatterResult.skipPDF = false;
        pdf = glm::dot(hitResult.hitWorldNormal, scatterDirection) / PI;
        return true;
    }

    double scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const override
    {
        double cosine = glm::dot(hitResult.hitWorldNormal, rayOut.direction);
        return cosine < 0 ? 0.f : (cosine / PI);
    }

public:
    glm::dvec3 albedo{};
    std::shared_ptr<Texture> texture = nullptr;
};

class LambertianTextured : public Material
{
public:
    LambertianTextured(std::shared_ptr<Texture> tex) : texture(tex) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, double &pdf) const override
    {
        glm::dvec3 scatterDirection = arc::randomInHemisphere(hitResult.hitWorldNormal);

        if (arc::isNearZero(scatterDirection))
            scatterDirection = hitResult.hitWorldNormal;
        scatterResult.attenuation = texture->sample(hitResult.hitUV.x, hitResult.hitUV.y);
        scatterResult.rayOut = Ray(hitResult.hitWorldPosition, scatterDirection);
        scatterResult.skipPDF = false;
        pdf = (glm::dot(hitResult.hitWorldNormal, scatterDirection) / PI);
        return true;
    }

    double scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const override
    {
        double cosine = glm::dot(hitResult.hitWorldNormal, rayOut.direction);
        return cosine < 0.f ? 0.f : static_cast<double>(cosine / PI);
    }

public:
    std::shared_ptr<Texture> texture = nullptr;
};

class Mirror : public Material
{
public:
    Mirror() = default;
    Mirror(const glm::dvec3 &color) : albedo(color) {}

    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatterResult, double &pdf) const override
    {
        scatterResult.attenuation = albedo;
        scatterResult.rayOut.direction = glm::reflect(ray.direction, hitResult.hitWorldNormal);
        scatterResult.rayOut.origin = hitResult.hitWorldPosition;
        scatterResult.skipPDF = true;
        return true;
    }

private:
    glm::dvec3 albedo{};
};

class LightSource : public Material
{
public:
    LightSource() = default;
    LightSource(const glm::dvec3 &radiance) : radiance(radiance) {}

    glm::dvec3 emit() const override
    {
        return radiance;
    }

private:
    // light energy
    glm::dvec3 radiance{};
};

class Metal : public Material
{
public:
    Metal(const glm::dvec3 &albedo, double specular)
        : albedo(albedo), specularExp(specular) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatteredResult, double &pdf) const override
    {
        glm::dvec3 reflected = glm::reflect(ray.direction, hitResult.hitWorldNormal);
        scatteredResult.rayOut = Ray(hitResult.hitWorldPosition, reflected + specularExp * arc::randomUnitVector());
        scatteredResult.attenuation = albedo;
        return glm::dot(scatteredResult.rayOut.direction, hitResult.hitWorldNormal) > 0.f;
    }

private:
    glm::dvec3 albedo{};
    double specularExp;
};

class Glass : public Material
{
public:
    Glass(double _ir) : ir(_ir) {}
    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatteredResult, double &pdf) const override
    {
        scatteredResult.attenuation = {1., 1., 1.};
        double refracetionRatio = glm::dot(ray.direction, hitResult.hitWorldNormal) < 0 ? (1.0 / ir) : ir;

        glm::dvec3 unitDirection = glm::normalize(ray.direction);
        glm::dvec3 refracted = glm::refract(unitDirection, hitResult.hitWorldNormal, refracetionRatio);

        scatteredResult.rayOut = Ray(hitResult.hitWorldPosition, refracted);
        return true;
    }

private:
    double ir;
};

class GlossyMaterial : public Material
{
public:
    GlossyMaterial(const glm::dvec3 &albedo, double roughness)
        : m_Albedo(albedo), m_Roughness(roughness) {}

    bool scatter(const Ray &ray, const HitResult &hitResult, ScatteredResult &scatteredResult, double &pdf) const override
    {
        glm::dvec3 halfway = glm::normalize(ray.direction + hitResult.hitWorldNormal);
        double nDotH = std::max(0.0, glm::dot(hitResult.hitWorldNormal, halfway));

        // Cook-Torrance microfacet BRDF
        double F = fresnel(glm::dot(ray.direction, halfway), 0.04f);
        double G = smithGGXCorrelated(glm::dot(hitResult.hitWorldNormal, ray.direction), glm::dot(hitResult.hitWorldNormal, glm::normalize(ray.direction)), m_Roughness);
        double D = GGX(nDotH, m_Roughness);

        double brdf = (F * G * D) / (4.0f * glm::dot(hitResult.hitWorldNormal, ray.direction) * glm::dot(hitResult.hitWorldNormal, glm::normalize(ray.direction)));

        glm::dvec3 reflectDir = glm::reflect(-ray.direction, hitResult.hitWorldNormal);
        scatteredResult.rayOut = Ray(hitResult.hitWorldPosition, reflectDir);
        scatteredResult.attenuation = m_Albedo * brdf;
        scatteredResult.skipPDF = false;
        pdf = static_cast<double>(brdf * glm::dot(hitResult.hitWorldNormal, glm::normalize(scatteredResult.rayOut.direction)) / PI);
        return true;
    }

    double scatterPDF(const Ray &rayIn, const HitResult &hitResult, const Ray &rayOut) const override
    {
        glm::dvec3 halfway = glm::normalize(rayIn.direction + rayOut.direction);
        double nDotH = std::max(0.0, glm::dot(hitResult.hitWorldNormal, halfway));
        double brdf = GGX(nDotH, m_Roughness);
        return static_cast<double>(brdf * glm::dot(hitResult.hitWorldNormal, glm::normalize(rayOut.direction)) / PI);
    }

private:
    glm::dvec3 m_Albedo;
    double m_Roughness;

    static double fresnel(double cosTheta, double f0)
    {
        return f0 + (1.0f - f0) * std::pow(1.0f - cosTheta, 5.0f);
    }

    static double GGX(double nDotH, double roughness)
    {
        double a = roughness * roughness;
        double a2 = a * a;
        double nDotH2 = nDotH * nDotH;
        return a2 / (PI * (nDotH2 * (a2 - 1.0f) + 1.0f) * (nDotH2 * (a2 - 1.0f) + 1.0f));
    }

    static double smithGGXCorrelated(double nDotV, double nDotL, double roughness)
    {
        double a = roughness * roughness;
        double nDotV2 = nDotV * nDotV;
        double nDotL2 = nDotL * nDotL;
        return 0.5f / (nDotV + std::sqrt(a + nDotV2 * (1.0f - a))) * 0.5f / (nDotL + std::sqrt(a + nDotL2 * (1.0f - a)));
    }
};

#endif