#pragma once
#include <glm/glm.hpp>
struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texcoord;
  glm::vec3 tangent;
  glm::vec3 bitangent;
};
