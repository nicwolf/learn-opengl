#include <glm/glm.hpp>

struct PointLight {
    glm::vec3 position;
    float pad12;

    glm::vec3 ambient;
    float pad28;
    glm::vec3 diffuse;
    float pad44;
    glm::vec3 specular;
    float pad60;

    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
    float pad76;
};

struct DirectionalLight {
    glm::vec3 direction;
    float pad12;

    glm::vec3 ambient;
    float pad28;
    glm::vec3 diffuse;
    float pad44;
    glm::vec3 specular;
    float pad60;
};

struct ConeLight {
    glm::vec3 position;
    float pad12;
    glm::vec3 direction;
    float pad28;

    glm::vec3 ambient;
    float pad44;
    glm::vec3 diffuse;
    float pad60;
    glm::vec3 specular;
    // Next FLOAT gets packed here

    float cutoff;
    float outerCutoff;
    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
};
