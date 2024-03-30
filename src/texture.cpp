#include "texture.hpp"

// std
#include <execution>

// thirdparty
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string &imagePath)
{
    int dummy = perPixelByte;
    imageData = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &dummy, perPixelByte);
    if (imageData == nullptr)
    {
        throw std::runtime_error("failed to create texture from " + imagePath);
    }
    perLine = imageWidth * perPixelByte;
}

Texture::~Texture()
{
    if (imageData)
        STBI_FREE(imageData);
}

glm::dvec3 Texture::sample(double u, double v)
{
    // default value for sampling and debugging
    if (imageData == nullptr)
        return albedo;

    u = glm::clamp(u, 0., 1.);
    v = glm::clamp(v, 0., 1.);

    int x = static_cast<int>(u * imageWidth);
    int y = static_cast<int>(v * imageHeight);
    auto result = pixelData(u, v);
    auto scale = 1.f / 255.f;
    return glm::dvec3(scale * result[0], scale * result[1], scale * result[2]);
}

const unsigned char *Texture::pixelData(int x, int y)
{
    static unsigned char defaultColor[] = {255, 0, 255};
    if (imageData == nullptr)
        return defaultColor;

    x = glm::clamp(x, 0, imageWidth);
    y = glm::clamp(y, 0, imageHeight);

    return imageData + y * perLine + x * perPixelByte;
}