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

const int WIDTH = 800;
const int HEIGHT = 600;

int main(int, char **)
{
    Camera camera(20.1143f, 0.1f, 100.f, 1280, 720);
    camera.setViewTarget(glm::vec3(20.f, 5.2f, 1.23612e-06), glm::vec3(0.f, 2.8f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    // Renderer renderer(WIDTH, HEIGHT);
    Renderer renderer;
    Model model("../models/cornell-box/cornell-box.obj");

    renderer.render(camera);

    return 0;
}
