#pragma once

#include "Decorator.h"

namespace BrainTree
{

// The Succeeder decorator returns success, regardless of what happens to the child.
class Succeeder : public Decorator
{
public:
    EStatus update(_float fTimeDelta) override
    {
        child->tick(fTimeDelta);
        return EStatus::Success;
    }
};

}