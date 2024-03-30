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
    std::srand((unsigned int)std::time(nullptr));
    Camera camera("../models/veach-mis/veach-mis.xml");
    camera.setSamplePerPixel(1000);

    Model model("../models/veach-mis/veach-mis.obj", camera.getLights());
    std::cout << "Scene lights: " << model.getLights().size() << std::endl;
    Scene scene(model.getTriangles(), model.getMaterials(), model.getLights());
    Renderer renderer(camera, scene);
    renderer.setBoudingTime(10);
    renderer.render();

    return 0;
}
