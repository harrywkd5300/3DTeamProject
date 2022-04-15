#pragma once

#include "Decorator.h"

namespace BrainTree
{

// The UntilSuccess decorator repeats until the child returns success and then returns success.
class UntilSuccess : public Decorator
{
public:
    EStatus update(_float fTimeDelta) override
    {
        while (1) {
            auto status = child->tick(fTimeDelta);

            if (status == EStatus::Success) {
                return EStatus::Success;
            }
        }
    }
};

}