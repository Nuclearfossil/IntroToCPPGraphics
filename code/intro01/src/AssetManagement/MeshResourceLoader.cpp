///
/// MeshResourceLoader.cpp - Source code for working with Mesh Resources
///

#include "stdafx.h"
#include "MeshResourceLoader.h"
#include "Graphics\Model.h"
#include "Graphics\Mesh.h"

#include "assimp\cimport.h"
#include "assimp\scene.h"

#include "utils\utils.h"
#include "utils\memory.h"


#include <d3d11.h>

MeshResourceLoader::MeshResourceLoader()
{
}

MeshResourceLoader::~MeshResourceLoader()
{
}

Model* MeshResourceLoader::Load(const aiScene* scene)
{
    // From the scene, load up the Meshs in the hierarchy
    int meshIndex = 0;
    Model* model = new Model();
    model->Initialize(scene->mNumMeshes);

    while (meshIndex < scene->mNumMeshes)
    {
        aiMesh* currentMesh = scene->mMeshes[meshIndex];
        
        unsigned int vertexCount = currentMesh->mNumVertices;
        unsigned int indexCount = currentMesh->mNumFaces * 3;
        
        PositionNormalUVLayout* vertexData = new PositionNormalUVLayout[vertexCount];

        for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
        {
            vertexData[vertexIndex].Position.x = currentMesh->mVertices[vertexIndex].x;
            vertexData[vertexIndex].Position.y = currentMesh->mVertices[vertexIndex].y;
            vertexData[vertexIndex].Position.z = currentMesh->mVertices[vertexIndex].z;
            vertexData[vertexIndex].Normal.x = currentMesh->mNormals[vertexIndex].x;
            vertexData[vertexIndex].Normal.y = currentMesh->mNormals[vertexIndex].y;
            vertexData[vertexIndex].Normal.z = currentMesh->mNormals[vertexIndex].z;
            vertexData[vertexIndex].UV.x = currentMesh->mTextureCoords[0][vertexIndex].x;
            vertexData[vertexIndex].UV.y = currentMesh->mTextureCoords[0][vertexIndex].y;
        }

        unsigned int* indexData = new unsigned int[indexCount+1];
        unsigned int faceCount = currentMesh->mNumFaces;
        unsigned int indexOffset = 0;
        for (int faceIndex = 0; faceIndex < faceCount; faceIndex++)
        {
            indexData[++indexOffset] = currentMesh->mFaces[faceIndex].mIndices[0];
            indexData[++indexOffset] = currentMesh->mFaces[faceIndex].mIndices[1];
            indexData[++indexOffset] = currentMesh->mFaces[faceIndex].mIndices[2];
        }

        Mesh* drawable = new Mesh();
        drawable->Load(vertexData, vertexCount, indexData, indexCount);
        
        model->AddMesh(drawable);

        meshIndex++;
    }

    return model;
}