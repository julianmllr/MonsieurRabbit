#pragma once

#include <cppgl.h>

class Particles {
    const uint32_t N;
    std::vector<glm::vec3> position;
    std::vector<glm::vec3> direction;
    std::vector<glm::vec4> color;
    std::vector<int> lifetime;
    std::vector<uint32_t> index_buffer;
    uint32_t vbo_id_pos, vbo_id_life, vbo_id_col;
    uint32_t start, end;
    const float particle_size;
    Mesh mesh;
    static Shader draw_shader;
    static Shader splat_shader;
    static Texture2D acid_mask;
    static Texture2D fire_mask;
    static Texture2D frost_mask;
    static Texture2D magic_mask;
    static Texture2D cloud_mask;

    uint32_t advance(uint32_t& pointer);

public:
    Particles(unsigned int N, float particle_size);
    ~Particles();

    void add(const glm::vec3 &pos, const glm::vec3& dir, const glm::vec4 &col, int alive_for_ms, int amount, bool random);
    void update();
    void draw();
};

// draw random float in [-1, 1]
inline float random_float() { return (float(rand() % 32768) / 16384.0f) - 1.0f; }
inline glm::vec2 random_vec2() { return glm::vec2(random_float(), random_float()); }
inline glm::vec3 random_vec3() { return glm::vec3(random_float(), random_float(), random_float()); }

extern std::shared_ptr<Particles> particles_small, particles;
