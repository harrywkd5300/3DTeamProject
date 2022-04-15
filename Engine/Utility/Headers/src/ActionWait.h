#pragma once

#include "Engine_Defines.h"
#include "BrainTree.h"

BEGIN(Engine)

class WaitNode : public BrainTree::Leaf
{
public:
	// note: using a blackboard is optional
	WaitNode(BrainTree::Blackboard::Ptr board) : Leaf(board) {}

	void initialize() override
	{
		fTime = 0;
	}

	EStatus update(_float fTimeDelta) override
	{
		fTime+= fTimeDelta;
		if (fTime >= limit) {
			return EStatus::Success;
		}

		return EStatus::Running;
	}

private:
	float limit =1.f;
	float fTime = 0.f;
};

END