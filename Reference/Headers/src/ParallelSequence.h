#pragma once

#include "Composite.h"

namespace BrainTree
{

class ParallelSequence : public Composite
{
public:
    ParallelSequence(bool success_on_all = true, bool fail_on_all = true) : use_success_fail_policy(true), success_on_all(success_on_all), fail_on_all(fail_on_all) {}
    ParallelSequence(int min_success, int min_fail) : min_success(min_success), min_fail(min_fail) {}

    EStatus update(_float fTimeDelta) override
    {
        int minimum_success = min_success;
        int minimum_fail = min_fail;

        if (use_success_fail_policy) {
            if (success_on_all) {
                minimum_success = (int)children.size();
            }
            else {
                minimum_success = 1;
            }

            if (fail_on_all) {
                minimum_fail = (int)children.size();
            }
            else {
                minimum_fail = 1;
            }
        }

        int total_success = 0;
        int total_fail = 0;

        for (auto &child : children) {
            auto status = child->tick(fTimeDelta);
            if (status == EStatus::Success) {
                total_success++;
            }
            if (status == EStatus::Failure) {
                total_fail++;
            }
        }

        if (total_success >= minimum_success) {
            return EStatus::Success;
        }
        if (total_fail >= minimum_fail) {
            return EStatus::Failure;
        }

        return EStatus::Running;
    }

private:
    bool use_success_fail_policy = false;
    bool success_on_all = true;
    bool fail_on_all = true;
    int min_success = 0;
    int min_fail = 0;
};

}