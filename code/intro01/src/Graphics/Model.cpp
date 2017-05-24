#include "stdafx.h"

#include "Mesh.h"
#include "Model.h"
#include "Material.h"

#include "utils\assert.h"


Model::Model()
{
    mMeshArray = nullptr;
    mMaterial = nullptr;

    mMeshCount = 0;
}

Model::~Model()
{
    for (unsigned int index = 0; index < mMeshCount; index++)
    {
        delete mMeshArray[index];
        mMeshArray[index] = nullptr;
    }

    delete[] mMeshArray;
    delete mMaterial;
}

void Model::Initialize(unsigned int meshcount)
{
    ASSERT(meshcount != 0);

    mMeshArray = new Mesh*[meshcount];
    for (unsigned int index = 0; index < meshcount; index++)
    {
        mMeshArray[index] = nullptr;
    }

    mMeshCount = meshcount;
}

bool Model::AddMesh(Mesh* mesh)
{
    ASSERT(mMeshCount != 0);

    bool result = false;

    // look for the first empty node and add it into that slot
    for (unsigned int index = 0; index < mMeshCount; index++)
    {
        if (mMeshArray[index] == nullptr)
        {
            mMeshArray[index] = mesh;
            result = true;
            break;
        }
    }

    return result;
}
