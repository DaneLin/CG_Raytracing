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
    Camera camera("../models/veach-mis/veach-mis.xml");
    camera.setSamplePerPixel(10);

    Model model("../models/veach-mis/veach-mis.obj");
    model.loadMaterials(camera.getLights());
    std::vector<Triangle> triangles;
    model.getTriangles(triangles);
    Scene scene(triangles, model.getMaterials());
    Renderer renderer(camera, scene);
    renderer.setBoudingTime(5);
    renderer.render();

    return 0;
}
