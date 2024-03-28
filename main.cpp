#include <iostream>
#include <vector>
#include <random>
#include <memory>
#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
// third party
#include <glm/glm.hpp>

#include "ray.hpp"
#include "renderer.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "scene.hpp"

int main(int, char **)
{
    Camera camera("../models/cornell-box/cornell-box.xml");
    camera.setSamplePerPixel(1000);

    Model model("../models/vikingroom/leftwall.obj");

    std::vector<Triangle> triangles;
    model.getTriangles(triangles);
    Scene scene(triangles, model.getMaterials());
    Renderer renderer(camera, scene);
    renderer.setBoudingTime(5);
    renderer.render();

    return 0;
}
