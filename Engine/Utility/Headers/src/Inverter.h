#pragma once

#include "Decorator.h"

namespace BrainTree
{

// The Inverter decorator inverts the child node's status, i.e. failure becomes success and success becomes failure.
// If the child runs, the Inverter returns the status that it is running too.
class Inverter : public Decorator
{
public:
    EStatus update(_float fTimeDelta) override
    {
        auto s = child->tick(fTimeDelta);

        if (s == EStatus::Success) {
            return EStatus::Failure;
        }
        else if (s == EStatus::Failure) {
            return EStatus::Success;
        }

        return s;
    }
};

}