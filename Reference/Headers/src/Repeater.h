#pragma once

#include "Decorator.h"

namespace BrainTree
{

// The Repeater decorator repeats infinitely or to a limit until the child returns success.
class Repeater : public Decorator
{
public:
    Repeater(int limit = 0) : limit(limit) {}

    void initialize() override
    {
        counter = 0;
    }

    EStatus update(_float fTimeDelta) override
    {
        while (1) {
            auto s = child->tick(fTimeDelta);

            if (s == EStatus::Running) {
                return EStatus::Running;
            }

            if (s == EStatus::Failure) {
                return EStatus::Failure;
            }

            if (limit > 0 && ++counter == limit) {
                return EStatus::Success;
            }

            child->reset();
        }
    }

protected:
    int limit;
    int counter = 0;
};

}