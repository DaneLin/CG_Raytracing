#include <iostream>
#include <vector>
#include <random>
#include <memory>
#include <ctime>

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
    std::srand(std::time(nullptr));
    Camera camera("../models/cornell-box/cornell-box.xml");
    camera.setSamplePerPixel(100);

    Model model("../models/vikingroom/leftwall.obj", camera.getLights());

    std::vector<Triangle> triangles;
    model.getTriangles(triangles);
    Scene scene(triangles, model.getMaterials(), model.getLights());
    Renderer renderer(camera, scene);
    renderer.setBoudingTime(10);
    renderer.render();

    return 0;
}
