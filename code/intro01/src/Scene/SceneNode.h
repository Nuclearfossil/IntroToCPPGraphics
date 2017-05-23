#pragma once

#include "EASTL\vector.h"

class SceneNode
{
public:
    SceneNode();
    ~SceneNode();

public:
    eastl::vector<SceneNode> mChildren;
};

