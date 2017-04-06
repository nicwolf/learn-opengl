#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL.h>

#include "shader.h"
#include "mesh.h"


class Model
{
public:
    Model(GLchar* path);
    void Draw(Shader shader);
private:
    std::vector<Mesh> meshes;
    std::string directory;
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                              aiTextureType type,
                                              std::string typeName);
};

#endif // MODEL_H
