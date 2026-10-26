#pragma once

#include <cppgl.h>
#include "renderer.h"

using namespace std;

class Scene {
public:
    Scene();
    ~Scene();

    void loadCubemap(vector<string> faces);
    void draw();

    // cubemap 
    unsigned int cubemapTexture;
    Mesh cube_mesh;
    Shader cube_shader;
};

extern std::shared_ptr<Scene> the_scene;
