#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>

#include "ray.hpp"

// std
#include <vector>
#include <string>

class Camera
{
public:
    Camera(float fvoy, float near, float far, uint32_t width, uint32_t height);
    Camera(const std::string &filepath);
    void setViewTarget(glm::dvec3 cameraPosition, glm::dvec3 targetCenter, glm::dvec3 upDiection);
    void setSamplePerPixel(uint32_t spp) { samplePerPixel = spp; }
    const uint32_t getWidth() const { return imageWidth; }
    const uint32_t getHeight() const { return imageHeight; }
    const glm::dvec3 &getPosition() const { return position; }
    const glm::mat4 &getView() const { return view; }
    const glm::mat4 &getInverseView() const { return inverseView; }
    const glm::mat4 &getProjection() const { return projection; }
    const glm::mat4 &getInverseProjection() const { return inversePorejction; }

    std::vector<glm::dvec3> getLights() const;

    Ray getRay(uint32_t i, uint32_t j);
    uint32_t getSampleCount() const { return samplePerPixel; }

private:
    void initialize();
    glm::dvec3 uniformSampleSquare();

private:
    glm::dvec3 position{};
    glm::dvec3 lookAt{};
    glm::dvec3 upDirection{};

    double focusDis = 1.f;
    double fvoy;
    double zNear, zFar;
    uint32_t samplePerPixel = 1;
    uint32_t imageWidth, imageHeight;

    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::mat4 inverseView{1.f};
    glm::mat4 inversePorejction{1.f};

    std::vector<glm::dvec3> rayDirections;
    std::vector<glm::dvec3> lights;

    glm::dvec3 delta_u{};
    glm::dvec3 delta_v{};
    glm::dvec3 pixelZeroLoc{};
};

#endif // __CAMERA_H__