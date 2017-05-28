///
/// AssetManager.h - General Asset Manager System.
/// Manage general assets in the system - allows for generation, querying, tracking and reporting
/// 
#pragma once

#include <unordered_map>
#include <string>

class IResourceLoader;
class Model;
class ShaderResource;

class AssetManager
{
    friend class IResourceLoader;
public:
    AssetManager();
    ~AssetManager();

    void Initialize();

    bool AddPath(const char* pathname);
    bool LoadModel(const char* filename);
    bool LoadShader(const char* filename, const char* shadermodel, const char* entrypoint);

    Model* GetModel(const char* filename);
    ShaderResource* GetShader(const char* filename);

private:
    bool GetPathToResource(const char* resource, char* dest);

private:
    std::string                 mBasePath;
    std::vector<std::string>    mPaths;

    std::unordered_map<std::string, Model*> mModels;
    std::unordered_map<std::string, ShaderResource*> mShaders;
};
