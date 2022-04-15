#pragma once

#include "Decorator.h"

namespace BrainTree
{

// The Failer decorator returns failure, regardless of what happens to the child.
class Failer : public Decorator
{
public:
    EStatus update(_float fTimeDelta) override
    {
        child->tick(fTimeDelta);
        return EStatus::Failure;
    }
};

}