#pragma once

#include "Composite.h"

namespace BrainTree
{

// The Selector composite ticks each child node in order.
// If a child succeeds or runs, the sequence returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children fails, only then does the selector fail.
class Selector : public Composite
{
public:
    void initialize() override
    {
        index = 0;
    }

    EStatus update(_float fTimeDelta) override
    {
        if (!has_children()) {
            return EStatus::Success;
        }

        // Keep going until a child behavior says it's running.
        while (1) {
            auto child = children.at(index);
            auto status = child->tick(fTimeDelta);

            // If the child succeeds, or keeps running, do the same.
            if (status != EStatus::Failure) {
                return status;
            }

            // Hit the end of the array, it didn't end well...
            if (++index == children.size()) {
                return EStatus::Failure;
            }
        }
    }
};

}