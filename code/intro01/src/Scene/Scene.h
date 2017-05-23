#pragma once
class Scene
{
public:
    Scene();
    ~Scene();

    void Add(Scene* child);
    void Remove(Scene* child);
};

