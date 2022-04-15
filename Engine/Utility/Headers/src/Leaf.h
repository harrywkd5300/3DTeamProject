#pragma once

#include "Node.h"
#include "Blackboard.h"

namespace BrainTree
{

class Leaf : public Node
{
public:
    Leaf() {}
    virtual ~Leaf() {}
    Leaf(Blackboard::Ptr blackboard) : blackboard(blackboard) {}
    
    virtual EStatus update(_float fTimeDelta) = 0;

protected:
	Blackboard::Ptr blackboard;
};

}
