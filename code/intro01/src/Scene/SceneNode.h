#pragma once

#include <vector>

class SceneNode
{
public:
    SceneNode();
    ~SceneNode();

public:
    std::vector<SceneNode> mChildren;
};

