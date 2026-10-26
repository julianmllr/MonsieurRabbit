#pragma once

#include <cppgl.h>

namespace rendering {
    struct PointLight {
        glm::vec3 pos;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float radius;
    };

    struct DirectionalLight {
        glm::vec3 dir;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    extern const DirectionalLight default_light;
    extern const PointLight point_light;
}

namespace rendering::util {
    // draw random floats in [-1, 1]
    inline float random_float() { return (float(rand() % 32768) / 16384.0f) - 1.0f; }
    inline glm::vec2 random_vec2() { return glm::vec2(random_float(), random_float()); }
    inline glm::vec3 random_vec3() { return glm::vec3(random_float(), random_float(), random_float()); }

    void setup_camera(const Shader& shader, const Camera& cam);
    void setup_light(const Shader&, const DirectionalLight&);
    void setup_light(const Shader& shader, const PointLight& light);
    void set_grayscale(const Shader& shader, bool gray);
}
