#include "buffer.h"

#include <cppgl.h>

using namespace glm;
using namespace rendering;

Framebuffer buffer::geom;
Framebuffer buffer::lighting;

static void clamp_to_edge(const Texture2D& tex) {
    glTextureParameteri(tex->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(tex->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void buffer::init() {
    const ivec2 res = Context::resolution();

    {
        geom = Framebuffer { "geom", res.x, res.y };

        geom->attach_depthbuffer(
            Texture2D { "geom.depth", res.x, res.y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT }
        );
        geom->attach_colorbuffer(Texture2D { "geom.diff", res.x, res.y, GL_RGB32F, GL_RGB, GL_FLOAT });
        geom->attach_colorbuffer(Texture2D { "geom.pos",  res.x, res.y, GL_RGB32F, GL_RGB, GL_FLOAT });
        geom->attach_colorbuffer(Texture2D { "geom.norm", res.x, res.y, GL_RGB32F, GL_RGB, GL_FLOAT });

        clamp_to_edge(geom->depth_texture);
        for (const auto& tex : geom->color_textures) {
            clamp_to_edge(tex);
        }

        geom->check();
    }

    {
        lighting = Framebuffer { "lighting", res.x, res.y };

        lighting->attach_depthbuffer(
            Texture2D { "lighting.depth", res.x, res.y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT }
        );
        lighting->attach_colorbuffer(Texture2D { "lighting.color", res.x, res.y, GL_RGB32F, GL_RGB, GL_FLOAT });

        clamp_to_edge(lighting->depth_texture);
        for (const auto& tex : lighting->color_textures) {
            clamp_to_edge(tex);
        }

        lighting->check();
    }

}

