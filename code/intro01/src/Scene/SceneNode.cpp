#include "SceneNode.h"

const size_t kINITIAL_CHILD_COUNT = 4;


SceneNode::SceneNode()
{
    mChildren.reserve(kINITIAL_CHILD_COUNT);
}


SceneNode::~SceneNode()
{
}

