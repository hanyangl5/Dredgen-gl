#include <glad/glad.h>
#include "RenderEngine.h"
#include "Log.h"
#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <stb_image.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui.h>

RenderEngine::RenderEngine(uint32_t _width, uint32_t _height) {
  width = _width;
  height = _height;
  Initglad();
  Init();
}

RenderEngine::~RenderEngine() {}

void RenderEngine::Update() {
  // ubolight->Update(light[0]);
}

void RenderEngine::Render() {

  // shadowpass->Draw(scene);

  deferred_pass->Draw(base_fbo, scene, main_cam, quad);
  //ao_pass->Draw(base_fbo, deferred_pass->PosTex(),deferred_pass->NormalTex(), quad);
  //postprocess_pass->Draw(base_fbo, quad); // take color buffer as input
  //skybox->Draw(base_fbo,main_cam->GetViewMatrix(),main_cam->projection,deferred_pass->PosTex());
}
uint32_t RenderEngine::GetTexture() { return base_fbo->fbo; }
void RenderEngine::GetSceneStat() {

  // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
  // ImGuiWindowFlags_NoScrollWithMouse;
  ImGui::Begin("scene");
  ImGui::SliderFloat("ssao factor", &ao_pass->factor, 0.001f, 1.0f);
  static int s_selected = -1;
  int index = 0;
  static std::string k;
  {
    ImGui::BeginChild("left pane", ImVec2(300, 0), true);
    // int index = 0;
    for (auto &i : scene) {
      char label[128];
      // sprintf("%s",objects[i].directory.c_str());
      sprintf(label, "%s", i.first.c_str());
      if (ImGui::Selectable(label, s_selected == index)) {
        s_selected = index;
        k = i.first.c_str();
      }
      index++;
    };
    ImGui::EndChild();
  }

  ImGui::SameLine();
  if (s_selected != -1 && !k.empty()) {
    ImGui::BeginGroup();
    ImGui::BeginChild(
        "item view",
        ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1
                                                         // line below us
    ImGui::Text("Object %d", s_selected);
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("Transform")) {
        auto &chosen_obj = scene.at(k);
        ImGui::InputFloat3("position",
                           glm::value_ptr(chosen_obj->transform.pos));
        ImGui::SliderFloat3(
            "rotation", glm::value_ptr(chosen_obj->transform.rot), 0.0, 360);
        ImGui::SliderFloat3(
            "scale", glm::value_ptr(chosen_obj->transform.scale), 0.01, 5.0);
        // int* a = ;
        // ImGui::InputInt("rendermode",
        // reinterpret_cast<int*>(&chosen_obj.rendermode)); std::cout <<
        // chosen_obj.rendermode<<"\n";
      }
      ImGui::EndTabBar();
    }
    ImGui::EndChild();
    if (ImGui::Button("remove")) {
      // auto iter = scene.find(k);
      // if (iter != scene.end()) {
      //	scene.erase(iter);
      //}
    }
    ImGui::EndGroup();
  }

  ImGui::End();
}
void RenderEngine::Destroy() {}

void RenderEngine::AddModel(std::string name, std::string path) {
  scene.insert({name, std::make_shared<Model>(path)});
}

void RenderEngine::Initglad() {

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Log::Err("Failed to initialize GLAD");
  } else
    Log::Log("glad inited\n");
}

void RenderEngine::Init() {
  // camera
  main_cam = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
  main_cam->SetProjectionMatrix(glm::perspective(glm::radians(main_cam->Zoom),
                                                 (float)width / (float)height,
                                                 0.1f, 1000.0f));
  light.push_back(std::make_shared<DirectLight>(glm::vec3(1.0, 1.0, 1.0),
                                                glm::vec3(0.0, 2.0, 0.0),
                                                glm::vec3(0.0, -1.0, -1.0)));
  quad = std::make_shared<Quad>();
  base_fbo = std::make_shared<Framebuffer>(width, height);
  // shadowpass = std::make_shared<Shadowpass>(light[0]);
  deferred_pass = std::make_shared<Deferrdpass>(width, height);
  ao_pass = std::make_shared<Aopass>(width, height);
  postprocess_pass = std::make_shared<PostProcesspass>(width, height);

  // shaders.insert({ "modelshader",
  // std::make_shared<Shader>("../resources/shaders/modelvs.glsl",
  // "../resources/shaders/modelps.glsl") }); ubolight =
  // std::make_shared<UboLight>(shaders.at("modelshader"));

  skybox = std::make_shared<Skybox>("../resources/textures/CornellBox");
  scene.insert({ "helmet",std::make_shared<Model>("../resources/models/DamagedHelmet/DamagedHelmet.gltf")});
  // scene.insert({
  // "thorn",std::make_shared<Model>("../resources/models/thorn/thorn.gltf") });
  scene.insert({ "sponza", std::make_shared<Model>(
							  "../resources/models/Sponza/glTF/Sponza.gltf") });

}
