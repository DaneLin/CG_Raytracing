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
    void setViewTarget(glm::vec3 cameraPosition, glm::vec3 targetCenter, glm::vec3 upDiection);
    void setSamplePerPixel(uint32_t spp) { samplePerPixel = spp; }
    const float getWidth() const { return imageWidth; }
    const float getHeight() const { return imageHeight; }
    const glm::vec3 &getPosition() const { return position; }
    const glm::mat4 &getView() const { return view; }
    const glm::mat4 &getInverseView() const { return inverseView; }
    const glm::mat4 &getProjection() const { return projection; }
    const glm::mat4 &getInverseProjection() const { return inversePorejction; }

    std::vector<glm::vec3> getLights() const;

    Ray getRay(uint32_t i, uint32_t j);
    uint32_t getSampleCount() const { return samplePerPixel; }

private:
    void initialize();
    glm::vec3 uniformSampleSquare();

private:
    glm::vec3 position{};
    glm::vec3 lookAt{};
    glm::vec3 upDirection{};

    float focusDis = 1.f;
    float fvoy;
    float zNear, zFar;
    uint32_t samplePerPixel = 1;
    uint32_t imageWidth, imageHeight;

    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::mat4 inverseView{1.f};
    glm::mat4 inversePorejction{1.f};

    std::vector<glm::vec3> rayDirections;
    std::vector<glm::vec3> lights;

    glm::vec3 delta_u{};
    glm::vec3 delta_v{};
    glm::vec3 pixelZeroLoc{};
};

#endif // __CAMERA_H__