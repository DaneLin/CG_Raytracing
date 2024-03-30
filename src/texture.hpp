#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <glm/glm.hpp>

// std
#include <memory>
#include <string>

class Texture
{
public:
    Texture(const std::string &imagePath);
    ~Texture();
    glm::dvec3 sample(double u, double v);

    // setter
    void setPerPixelByte(int pbb) { perPixelByte = pbb; }

    // getter
    int getWidth() const { return imageWidth; }
    int getHeight() const { return imageHeight; }

private:
    const unsigned char *pixelData(int x, int y);

private:
    int perPixelByte = 3;
    int imageWidth, imageHeight;
    int perLine;
    unsigned char *imageData = nullptr;
    // default color for debugging.
    glm::dvec3 albedo{1, 0, 1};
};

#endif