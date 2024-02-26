#pragma once

#include <imgui.h>

namespace PetrolEngine {
  class ImGuiLayer {
    public:
    static void init();
    static void destroy();
    static void draw();
    static void clear();
  };
}
