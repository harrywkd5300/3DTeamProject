#pragma once

#include "Decorator.h"

namespace BrainTree
{

// The UntilFail decorator repeats until the child returns fail and then returns success.
class UntilFail : public Decorator
{
public:
    EStatus update(_float fTimeDelta) override
    {
        while (1) {
            auto status = child->tick(fTimeDelta);

            if (status == EStatus::Failure) {
                return EStatus::Success;
            }
        }
    }
};

}