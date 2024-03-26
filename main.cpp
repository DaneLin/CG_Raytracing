#include <iostream>
#include <vector>
#include <random>
#include <memory>

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
    camera.setSamplePerPixel(1);

    Model model("../models/cornell-box/leftwall.obj");

    std::vector<Triangle> triangles;
    model.getTriangles(triangles);
    Scene scene(triangles);
    Renderer renderer(camera, scene);
    renderer.render();

    return 0;
}
