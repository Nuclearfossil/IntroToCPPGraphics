///
/// AssetManager.cpp - source code for the AssetManager System
/// Wherein we track, create and release various assets necessary for the application.
///

#include "stdafx.h"
#include "AssetManager.h"
#include "IResourceLoader.h"
#include "utils\assert.h"

#include "assimp\cimport.h"
#include "assimp\scene.h"

#include "MeshResourceLoader.h"

#include "Graphics\Model.h"
#include "Graphics\ShaderResource.h"

#include "utils\utils.h"

#include <sstream>

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
    for (auto model : mModels)
        delete model.second;

    for (auto shader : mShaders)
        delete shader.second;
}

void AssetManager::Initialize()
{
    mBasePath = Pwd();
}

bool AssetManager::AddPath(const char* pathname)
{
    ASSERT(pathname != nullptr);

    bool result = false;

    // check to see if path exists before adding?
    if (Exists(pathname))
    {
        mPaths.push_back(std::string(pathname));
        result = (mPaths.size() > 0);
    }

    return result;
}

bool AssetManager::LoadModel(const char* filename)
{
    ASSERT(filename != nullptr);

    bool result = false;
    char filepath[1024];

    result = GetPathToResource(filename, filepath, 1024);

    // Build the asset, since the file exists
    if (result)
    {
        const aiScene* scene = aiImportFile(filepath, 0);
        
        // Construct away!
        if ((scene != nullptr)
            && scene->HasMeshes()
            && scene->HasMaterials())
        {            
            MeshResourceLoader meshLoader;
            Model* model = meshLoader.Load(scene);
            mModels[filename] = model;
        }
        else
        {
            // Some quick asserts to make sure we have data to work with
            ASSERT(scene != nullptr);
            ASSERT(scene->HasMeshes());
            ASSERT(scene->HasMaterials());
        }
        aiReleaseImport(scene);
    }
    return result;
}

Model* AssetManager::GetModel(const char* filename)
{
    if (mModels.find(filename) != mModels.end())
        return mModels[filename];

    return nullptr;
}

bool AssetManager::LoadShader(const char* filename, const char* shadermodel, const char* entrypoint)
{
    ASSERT(filename != nullptr);

    bool result = false;
    char filepath[1024];

    result = GetPathToResource(filename, filepath, 1024);

    // Build the asset, since the file exists
    if (result)
    {
        ShaderResource* shader = new ShaderResource();
        if (shader->LoadShader(filepath, shadermodel, entrypoint))
        {
            mShaders[filename] = shader;
            return true;
        }
    }

    return false;
}

bool AssetManager::GetPathToResource(const char* resource, char* dest, unsigned int size)
{
    ASSERT(resource != nullptr);
    ASSERT(dest != nullptr);

    bool result = false;
    std::string normalized;
    for (auto path : mPaths)
    {
        std::stringstream filePath;
        filePath << mBasePath << "\\" << path << "\\" << resource;
        if (Exists(filePath.str().c_str()))
        {
            normalized = filePath.str();
            strcpy(dest, normalized.c_str());
            result = true;
            break;
        }
    }

    return result;
}