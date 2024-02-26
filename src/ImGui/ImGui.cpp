#include "ImGui.h"
#include "Core/Aliases.h"
#include "Core/Components/Component.h"
#include "Core/Components/Properties.h"
#include "Core/Components/Transform.h"
#include "PCH.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <Static/Window/Window.h>
#include <Core/Components/Entity.h>
#include <Core/Scene.h>
#include <string>

namespace PetrolEngine {
  ImGuiIO* io;
  void ImGuiLayer::init(){
    ::IMGUI_CHECKVERSION();
    ::ImGui::CreateContext();
    io = &::ImGui::GetIO(); (void)*io;

    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ::ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ::ImGui::GetStyle();
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ::ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Window::i_window, true);
    ::ImGui_ImplOpenGL3_Init("#version 440");
  }
  void ImGuiLayer::destroy(){
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplGlfw_Shutdown();
    ::ImGui::DestroyContext();
  }
  Entity* selectedEntity = nullptr;
  void f(Entity* e){
    ImGui::PushID((int)e->getID());
    if(ImGui::TreeNode(e->getComponent<Properties>().name)){
      
      if(ImGui::IsItemClicked()) selectedEntity = e;

      for(Entity* c : e->children) f(c);

      ImGui::TreePop();
    }
    ImGui::PopID();
  }

  void ImGuiLayer::clear(){
    ::ImGui_ImplOpenGL3_NewFrame();
    ::ImGui_ImplGlfw_NewFrame();
    ::ImGui::NewFrame();

    
    ImGui::Begin("Entity List");

    for(auto* scene : loadedScenes){
      if(ImGui::TreeNode("scene")){
        for(Entity* e : scene->entities) if(e->parent == nullptr) f(e);
      
        ImGui::TreePop();
      }
    }
    ImGui::End();
    
    ImGui::Begin("Inspector");
    if(selectedEntity != nullptr){
      for(auto type : Entity::componentTypes){
        auto storage = selectedEntity->getScene()->sceneRegistry.storage(type);
        bool yes = storage->contains((entt::entity)selectedEntity->getID());
        InternalComponent* mydata = nullptr;
        if(yes) mydata = (InternalComponent*)storage->get((entt::entity)selectedEntity->getID());
        if(mydata == nullptr) continue;
        LOG(toString(mydata->inspectorTypes().size()), 3);
        auto clas = split(std::string(storage->type().name().data()), ':').back();
        clas.pop_back();
        ImGui::Text(clas.c_str());
        static bool isOpen= false;
        if(ImGui::CollapsingHeader("idC)
        for(auto types : mydata->inspectorTypes()){
          switch (types.second.first) {
            case InspectorType::Float : {ImGui::InputFloat (types.first.c_str(), (float*)((uint64)mydata + (uint64)types.second.second)); break;}
            case InspectorType::Float2: {ImGui::InputFloat2(types.first.c_str(), (float*)((uint64)mydata + (uint64)types.second.second)); break;}
            case InspectorType::Float3: {ImGui::InputFloat3(types.first.c_str(), (float*)((uint64)mydata + (uint64)types.second.second)); break;}
            case InspectorType::Float4: {ImGui::InputFloat4(types.first.c_str(), (float*)((uint64)mydata + (uint64)types.second.second)); break;}
            default: break;
          }
          //ImGui::InputFloat3(types.first.c_str(), (float*)((uint64)mydata + (uint64)types.second.second));
        }
      }
      //ImGui::InputFloat3(selectedEntity->getComponent<Properties>().name, &selectedEntity->getComponent<Transform>().position.x);
    }
    ImGui::End();
  }

  void ImGuiLayer::draw(){
    ::ImGui::Render();
    ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ::ImGui::UpdatePlatformWindows();
        ::ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
  }
}
