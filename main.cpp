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
    camera.setSamplePerPixel(2000);

    Model model("../models/cornell-box/cornell-box.obj", camera.getLights());
    std::cout << "Scene lights: " << model.getLights().size() << std::endl;
    Scene scene(model.getTriangles(), model.getMaterials(), model.getLights());
    Renderer renderer(camera, scene);
    renderer.setBoudingTime(50);
    renderer.render();

    return 0;
}
