#pragma once
#include "Engine_Defines.h"
#include <memory>


namespace BrainTree
{

class Node
{
public:
	using Ptr = std::shared_ptr<Node>;

    //enum class EStatus
    //{
    //    Invalid,
    //    Success,
    //    Failure,
    //    Running,
    //};

    virtual ~Node() {}

    virtual EStatus update(_float fTimeDelta) = 0;
    virtual void initialize() {}
    virtual void terminate(EStatus s) {}

    EStatus tick(_float fTimeDelta)
    {
        if (status != EStatus::Running) {
            initialize();
        }

        status = update(fTimeDelta);

        if (status != EStatus::Running) {
            terminate(status);
        }

        return status;
    }

    bool is_success() const { return status == EStatus::Success; }
    bool is_failure() const { return status == EStatus::Failure; }
    bool is_running() const { return status == EStatus::Running; }
    bool is_terminated() const { return is_success() || is_failure(); }
    void reset() { status = EStatus::Invalid; }

protected:
    EStatus status = EStatus::Invalid;
};

}