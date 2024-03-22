#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>

// std
#include <vector>

class Camera
{
public:
    Camera(float fvoy, float near, float far, uint32_t width, uint32_t height);

    void setViewTarget(glm::vec3 cameraPosition, glm::vec3 targetCenter, glm::vec3 upDiection);

    const float getWidth() const { return imageWidth; }
    const float getHeight() const { return imageHeight; }
    const glm::vec3 &getPosition() const { return position; }
    const glm::mat4 &getView() const { return view; }
    const glm::mat4 &getInverseView() const { return inverseView; }
    const glm::mat4 &getProjection() const { return projection; }
    const glm::mat4 &getInverseProjection() const { return inversePorejction; }

    const std::vector<glm::vec3> &getRayDirections() const { return rayDirections; }

private:
    void calcProjection();
    void calcView();
    void calcRayDirections();

private:
    glm::vec3 position{};
    glm::vec3 lookAt{};
    glm::vec3 upDirection{};

    float fvoy;
    float zNear, zFar;
    uint32_t imageWidth, imageHeight;

    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::mat4 inverseView{1.f};
    glm::mat4 inversePorejction{1.f};

    std::vector<glm::vec3> rayDirections;
};

#endif // __CAMERA_H__