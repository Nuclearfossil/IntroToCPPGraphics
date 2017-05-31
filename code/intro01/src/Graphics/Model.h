#pragma once

// ======================================================================================
// Forward Declarations - so the header file doesn't have to #include anything
// ======================================================================================
class Mesh;
class Material;

class Model
{
public:
    Model();
    ~Model();

    void Initialize(unsigned int meshcount);
    bool AddMesh(Mesh* mesh);

    void Render();

private:
    Mesh** mMeshArray;
    Material* mMaterial;

    unsigned int mMeshCount;
};
