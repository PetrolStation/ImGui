#pragma once

#include <imgui.h>
#include "Core/Renderer/Texture.h"
#include <OpenGL/OpenGL.h>

namespace PetrolEngine {
  class ImGuiLayer {
    public:
    static Entity* selectedEntity;
    static void init();
    static void destroy();
    static void drawInspector();
    static void drawEntityList();
    static void drawTexture(Texture* tex, GLuint RBO);
    static void draw();
    static void clear();
  };
}
