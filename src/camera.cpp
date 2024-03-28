#include "camera.hpp"
#include "helper.hpp"

// thirdparty
#include <glm/gtc/matrix_transform.hpp>
#include "tinyxml2.h"

// std
#include <random>
#include <iostream>
#include <exception>
#include <sstream>

Camera::Camera(float fvoy, float near, float far, uint32_t width, uint32_t height)
    : fvoy(fvoy), zNear(near), zFar(far), imageWidth(width), imageHeight(height)
{
}

Camera::Camera(const std::string &filepath)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result = doc.LoadFile(filepath.c_str());
    if (result != tinyxml2::XML_SUCCESS)
    {
        throw std::runtime_error("failed to read xml file!");
    }

    // 处理camera
    tinyxml2::XMLElement *pCamera = doc.FirstChildElement("camera");
    if (pCamera != nullptr)
    {
        const char *type = pCamera->Attribute("type");
        imageWidth = pCamera->IntAttribute("width");
        imageHeight = pCamera->IntAttribute("height");
        fvoy = pCamera->FloatAttribute("fovy");
        // 处理eye
        tinyxml2::XMLElement *pEye = pCamera->FirstChildElement("eye");
        if (pEye != nullptr)
        {
            float x = pEye->FloatAttribute("x");
            float y = pEye->FloatAttribute("y");
            float z = pEye->FloatAttribute("z");
            position = {x, y, z};
        }
        tinyxml2::XMLElement *pLookat = pCamera->FirstChildElement("lookat");
        if (pLookat != nullptr)
        {
            float x = pLookat->FloatAttribute("x");
            float y = pLookat->FloatAttribute("y");
            float z = pLookat->FloatAttribute("z");
            lookAt = {x, y, z};
        }
        tinyxml2::XMLElement *pUp = pCamera->FirstChildElement("up");
        if (pUp != nullptr)
        {
            float x = pUp->FloatAttribute("x");
            float y = pUp->FloatAttribute("y");
            float z = pUp->FloatAttribute("z");
            upDirection = {x, y, z};
        }
    }

    // 处理light
    int lightCount = doc.ChildElementCount("light");
    tinyxml2::XMLElement *pLight = doc.FirstChildElement("light");
    while (lightCount > 0)
    {
        const char *mtlname = pLight->Attribute("mtlname");
        const char *radiance = pLight->Attribute("radiance");
        glm::vec3 tmp{};
        std::stringstream ss(radiance);
        char delim;
        ss >> tmp.x >> delim >> tmp.y >> delim >> tmp.z;
        lights.push_back(tmp);
        // std::cout << tmp.x << ' ' << tmp.y << ' ' << tmp.z << std::endl;
        lightCount--;
        pLight = pLight->NextSiblingElement();
    }
    initialize();
}

std::vector<glm::vec3> Camera::getLights() const
{
    return lights;
}

void Camera::setViewTarget(glm::vec3 cameraPosition, glm::vec3 targetCenter, glm::vec3 upDiection)
{
    position = cameraPosition;
    lookAt = targetCenter;
    this->upDirection = upDiection;
    initialize();
}

void Camera::initialize()
{
    float radianAngle = glm::radians(fvoy);
    float height = glm::tan(radianAngle / 2);

    float projectionHeight = height * 2.f * focusDis;
    float projectionWidth = projectionHeight * ((float)imageWidth / (float)imageHeight);

    auto w = glm::normalize(position - lookAt);
    auto u = glm::normalize(glm::cross(upDirection, w));
    auto v = glm::cross(w, u);

    glm::vec3 u_ = projectionWidth * u;
    glm::vec3 v_ = projectionHeight * -v;

    delta_u = u_ / (float)imageWidth;
    delta_v = v_ / (float)imageHeight;

    pixelZeroLoc = position - (focusDis * w) - u_ / 2.f - v_ / 2.f;
    pixelZeroLoc += 0.5f * (delta_u + delta_v);
}

Ray Camera::getRay(uint32_t i, uint32_t j)
{
    glm::vec3 center = pixelZeroLoc + ((float)i * delta_u) + ((float)j * delta_v);
    glm::vec3 sampleCenter = center + uniformSampleSquare();

    return Ray(position, sampleCenter - position);
}

glm::vec3 Camera::uniformSampleSquare()
{
    float px = -0.5 + arc::randomDouble();
    float py = -0.5 + arc::randomDouble();
    return px * delta_u + py * delta_v;
}
