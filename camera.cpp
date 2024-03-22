#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fvoy, float near, float far, uint32_t width, uint32_t height)
    : fvoy(fvoy), zNear(near), zFar(far), imageWidth(width), imageHeight(height)
{
}

void Camera::setViewTarget(glm::vec3 cameraPosition, glm::vec3 targetCenter, glm::vec3 upDiection)
{
    position = cameraPosition;
    lookAt = targetCenter;
    this->upDirection = upDiection;
    calcView();
    calcProjection();
    calcRayDirections();
}

void Camera::calcProjection()
{
    projection = glm::perspectiveFov(fvoy, (float)imageWidth, (float)imageHeight, zNear, zFar);
    inversePorejction = glm::inverse(projection);
}

void Camera::calcView()
{
    view = glm::lookAt(position, lookAt, upDirection);
    inverseView = glm::inverse(view);
}

void Camera::calcRayDirections()
{
    rayDirections.resize(imageWidth * imageHeight);

    for (uint32_t y = 0; y < imageHeight; ++y)
    {
        for (uint32_t x = 0; x < imageWidth; ++x)
        {
            glm::vec2 imagePos = {(float)x / (float)imageWidth, (float)y / (float)imageHeight};
            imagePos = imagePos * 2.f - 1.f;

            glm::vec4 target = inversePorejction * glm::vec4(imagePos.x, imagePos.y, 1, 1);
            glm::vec3 rayDirection = glm::vec3(inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
            rayDirections[x + y * imageWidth] = rayDirection;
        }
    }
}
