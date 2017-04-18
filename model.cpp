#include "model.h"

std::vector<Texture> textures_loaded;
GLuint textureFromFile(const char * s, std::string directory);

Model::Model(GLchar* path)
{
    this->loadModel(path);
}

void Model::Draw(Shader shader)
{
    for (GLuint i = 0; i < this->meshes.size(); i ++)
    {
        this->meshes[i].Draw(shader);
    }
}

void Model::DrawInstanced(Shader shader, GLuint instanceCount)
{
    for (GLuint i = 0; i < this->meshes.size(); i ++)
    {
        this->meshes[i].DrawInstanced(shader, instanceCount);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene * scene;
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene
        || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    }
    this->directory = path.substr(0, path.find_last_of('/'));
    this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
    for (GLuint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }

    for (GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;

    // Process Vertices
    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 v;

        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        vertex.position = v;

        v.x = mesh->mNormals[i].x;
        v.y = mesh->mNormals[i].y;
        v.z = mesh->mNormals[i].z;
        vertex.normal = v;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 v;
            v.x = mesh->mTextureCoords[0][i].x;
            v.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = v;
        }
        else
        {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process Indices
    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j ++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process Materials
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

        // Diffuse Maps
        std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
                                                                      aiTextureType_DIFFUSE,
                                                                      "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // Specular Maps
        std::vector<Texture> specularMaps = this->loadMaterialTextures(material,
                                                                       aiTextureType_SPECULAR,
                                                                       "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString s;
        mat->GetTexture(type, i, &s);
        GLboolean skip = false;
        for (GLuint j = 0; j < textures_loaded.size(); j ++)
        {
            if (std::strcmp(textures_loaded[j].path.C_Str(), s.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.id = textureFromFile(s.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = s;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

GLuint textureFromFile(const char * filename, std::string directory) {
    std::string path = directory + '/' + std::string(filename);

    int width, height;
    unsigned char * textureImg = SOIL_load_image(path.c_str(),
                                                 &width, &height,
                                                 0, SOIL_LOAD_RGB);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImg);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SOIL_free_image_data(textureImg);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
