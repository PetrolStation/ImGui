#include "ImGui.h"
#include "Core/Aliases.h"
#include "Core/Components/Component.h"
#include "Core/Components/InspectorTypes.h"
#include "Core/Components/Properties.h"
#include "Core/Components/Transform.h"
#include "Core/DebugTools.h"
#include "PCH.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <Static/Window/Window.h>
#include <Core/Components/Entity.h>
#include <Core/Scene.h>
#include <bitset>
#include <memory>
#include <string>

namespace PetrolEngine {
  ImGuiIO* io;
  void ImGuiLayer::init(){ LOG_FUNCTION();
    ::IMGUI_CHECKVERSION();
    ::ImGui::CreateContext();
    io = &::ImGui::GetIO(); (void)*io;

    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io->ConfigDockingAlwaysTabBar = true;
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

  void ImGuiLayer::destroy(){ LOG_FUNCTION();
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplGlfw_Shutdown();
    ::ImGui::DestroyContext();
  }

  void ImGuiLayer::drawTexture(Texture* tex, GLuint RBO){
    ImGui::Begin("My Scene");
    
    // we access the ImGui window size
    const float window_width = ImGui::GetContentRegionAvail().x;
    const float window_height = ImGui::GetContentRegionAvail().y;
    
    // we rescale the framebuffer to the actual window size here and reset the glViewport 
    glBindTexture(GL_TEXTURE_2D, tex->getID());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getID(), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    glViewport(0, 0, window_width, window_height);
    
    // we get the screen position of the window
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // and here we can add our created texture as image to ImGui
    // unfortunately we need to use the cast to void* or I didn't find another way tbh
    ImGui::GetWindowDrawList()->AddImage(
        (void *)tex->getID(), 
        ImVec2(pos.x, pos.y), 
        ImVec2(pos.x + window_width, pos.y + window_height), 
        ImVec2(0, 1), 
        ImVec2(1, 0)
    );
    
    ImGui::End();

  }

  Entity* selectedEntity = nullptr;
  void entityTree(Entity* e){ LOG_FUNCTION();
    ImGui::PushID((int)e->getID());
    if(ImGui::TreeNode(e->name.c_str())){
      
      if(ImGui::IsItemClicked()) selectedEntity = e;

      for(Entity* c : e->children) entityTree(c);

      ImGui::TreePop();
    }
    ImGui::PopID();
  }

              //case InspectorType::Float : {ImGui::InputFloat (types.first.c_str(), (float*)((uint64)mydata + (uint64)types.second.second)); break;}

  void typeInput(InspectorType types[4], const char* name, void* data, int depth = 0){ LOG_FUNCTION();
    if(depth == 4) return;
    ImGui::PushID(depth);
    InspectorType type = types[depth];
    switch (type) {
      case InspectorType::Float : {ImGui::InputFloat (name, (float*)data); break;}
      case InspectorType::Float2: {ImGui::InputFloat2(name, (float*)data); break;}
      case InspectorType::Float3: {ImGui::InputFloat3(name, (float*)data); break;}
      case InspectorType::Float4: {ImGui::InputFloat4(name, (float*)data); break;}
      case InspectorType::Int   : {ImGui::InputInt   (name, (  int*)data); break;}
      case InspectorType::Int2  : {ImGui::InputInt2  (name, (  int*)data); break;}
      case InspectorType::Int3  : {ImGui::InputInt3  (name, (  int*)data); break;}
      case InspectorType::Int4  : {ImGui::InputInt4  (name, (  int*)data); break;}
      case InspectorType::Bool  : {ImGui::Checkbox   (name, ( bool*)data); break;}
      case InspectorType::Vector: {
        if(depth == 2) return;
        if(ImGui::TreeNode(name)){
        InspectorType inType = types[depth+1];
        std::vector<char>* vector = (std::vector<char>*)data;
        
        char* p = vector->data();
        for(int i = 0; i < vector->size()/getTypeSize(inType); i++){
          typeInput(types, toString(i).c_str(), (void*)p, depth+1);
          p += getTypeSize(inType);
        }

        if(ImGui::Button("add")){
          int size = getTypeSize(inType);
          for(int i =0; i<size;i++) vector->push_back((char)0);
          if(inType == InspectorType::Vector) *(std::vector<char>*)p = std::vector<char>();
        }
        ImGui::TreePop();
        }
        break;
      }
      case InspectorType::String: {
        String* text = (String*)data;
        ImGui::InputText  (name, text->data(), text->size());
        break;
      }
      default: break;
    }
    ImGui::PopID();
  }

  void ImGuiLayer::drawEntityList(){
    ImGui::Begin("Entity List");
    for(auto* scene : loadedScenes){
      if(ImGui::TreeNode("scene")){
        for(Entity* e : scene->entities) if(e->parent == nullptr) entityTree(e);
      
        ImGui::TreePop();
      }
    }
    ImGui::End();
    ImGui::ShowMetricsWindow();
  }

  void ImGuiLayer::drawInspector(){
    ImGui::Begin("Inspector");
    if(selectedEntity != nullptr){
      ImGui::InputText("Name", selectedEntity->name.data(), selectedEntity->name.size()+1);
      for(auto type : Entity::componentTypes){
        auto storage = selectedEntity->getScene()->sceneRegistry.storage(type);
        
        InternalComponent* mydata = nullptr;
        
        if(storage->contains((entt::entity)selectedEntity->getID()))
          mydata = (InternalComponent*)storage->value((entt::entity)selectedEntity->getID());
        
        if(mydata == nullptr) continue;
        
        auto clas = split(std::string(storage->type().name().data()), ':').back(); clas.pop_back();
        
        if(ImGui::CollapsingHeader(clas.c_str())){
          for(auto types : (*exposedElements)[mydata->typeId]){
            InspectorType type[4];
            memcpy(type, &types.type, sizeof(InspectorType[4]));
            typeInput(type, types.name, (void*)((uint64)mydata + (uint64)types.offset));
          }
        }
      }
    }
    ImGui::End();
  }

  void ImGuiLayer::clear(){ LOG_FUNCTION();
    ::ImGui_ImplOpenGL3_NewFrame();
    ::ImGui_ImplGlfw_NewFrame();
    ::ImGui::NewFrame();
    ::ImGui::DockSpaceOverViewport();
  }

  void ImGuiLayer::draw(){ LOG_FUNCTION();
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
