#include "scene.h"
#include <stbi/stb_image.h>

shared_ptr<Scene> the_scene;

Scene::Scene() : cubemapTexture(0) {
    // init skybox prototype
    // setup mesh
    glm::vec3 vertices[8] = { {-1,-1,-1}, {1,-1,-1}, {1,-1,1}, {-1,-1,1},
        {-1,1,-1}, {1,1,-1}, {1,1,1}, {-1,1,1} };
    unsigned int indices[36] = { 4, 0, 1, 1, 5, 4,
        3, 0, 4, 4, 7, 3,
        1, 2, 6, 6, 5, 1,
        3, 7, 6, 6, 2, 3,
        4, 5, 6, 6, 7, 4,
        0, 3, 1, 1, 3, 2 };
    cube_mesh = Mesh("skybox-mesh");
    cube_mesh->add_vertex_buffer(GL_FLOAT, 3, 8, vertices);
    cube_mesh->add_index_buffer(36, indices);

    // setup material
    // first back or front?
    std::vector<std::string> faces = { "render-data/skybox/right.jpg", "render-data/skybox/left.jpg", "render-data/skybox/bottom.jpg",
        "render-data/skybox/top.jpg", "render-data/skybox/front.jpg", "render-data/skybox/back.jpg" };
    loadCubemap(faces);

    // setup shader
    cube_shader = Shader("skybox-shader", "shader/skybox.vs", "shader/skybox.fs");
}

Scene::~Scene() { }

void Scene::loadCubemap(vector<string> faces)
{
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Scene::draw() {
    glDepthMask(GL_FALSE);
    cube_shader->bind(); //glUseProgram(id)
    // ... set view and projection matrix
    cube_shader->uniform("view", glm::mat4(glm::mat3(current_camera()->view)));
    cube_shader->uniform("proj", current_camera()->proj);
    cube_mesh->bind(cube_shader); // glBindVertexArray(vao)
    // set uniform (material->bind -> shader->uniform)
    int loc = glGetUniformLocation(cube_shader->id, "skybox"); // got rid of c_str
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(loc, 0);

    cube_mesh->draw();
    cube_mesh->unbind();
    cube_shader->unbind();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
}