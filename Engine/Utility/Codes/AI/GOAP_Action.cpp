#include "AI\GOAP_Action.h"
#include "AI\GOAP_WorldState.h"

#include <cassert>

USING(Engine)
#ifdef _KJW_AI_CONTAINER_ARRAY
CAction::CAction(const CAction & rhs)
{

	name_ = rhs.name_;
	cost_ = rhs.cost_;
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		preconditions_[i] = rhs.preconditions_[i];
	//memcpy(preconditions_, rhs.preconditions_, sizeof(_byte)*AI_ARRAY_SIZE_MAX);
	preconditions_Size = rhs.preconditions_Size;
	preconditions_Max_Number = rhs.preconditions_Max_Number;

	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		effects_[i] = rhs.effects_[i];
	//memcpy(effects_, rhs.effects_, sizeof(_byte)*AI_ARRAY_SIZE_MAX);
	effects_Size = rhs.effects_Size;
	effects_Max_Number = rhs.effects_Max_Number;
}
#endif
CAction::CAction() : cost_(0){
#ifdef _KJW_AI_CONTAINER_ARRAY
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		preconditions_[i] = 2;
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		effects_[i] = 2;

	//memset(preconditions_, 2, sizeof(byte) * sizeof(AI_ARRAY_SIZE_MAX));
	//memset(effects_, 2, sizeof(byte) * sizeof(AI_ARRAY_SIZE_MAX));
#endif

}

CAction::CAction(std::string name, int cost) : CAction() {
#ifdef _KJW_AI_CONTAINER_ARRAY
	//memset(preconditions_, 2, sizeof(byte) * sizeof(AI_ARRAY_SIZE_MAX));
	//memset(effects_, 2, sizeof(byte) * sizeof(AI_ARRAY_SIZE_MAX));
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		preconditions_[i] = 2;
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		effects_[i] = 2;
#endif
    // Because delegating constructors cannot initialize & delegate at the same time...
    name_ = name;
    cost_ = cost;
}

bool CAction::operableOn(const CWorldState& ws) const {

#ifdef _KJW_AI_CONTAINER_ARRAY
	
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i) {
		if (ws.vars_[i] == 2 || preconditions_[i] == 2)
			continue;

		if (ws.vars_[i] != preconditions_[i]) {
			return false;
		}
	}
	return true;
#else
	for (const auto& precond : preconditions_) {
		try {
			if (ws.vars_.at(precond.first) != precond.second) {
				return false;
			}
		}
		catch (const std::out_of_range&) {
			return false;
		}
	}
	return true;
#endif
}

CWorldState CAction::actOn(const CWorldState& ws) const {
#ifdef _KJW_AI_CONTAINER_ARRAY
    CWorldState tmp(ws);
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
	{
		if(effects_[i]==2)
			continue;

		tmp.setVariable(i, effects_[i]);
	}
	return tmp;
#else
	CWorldState tmp(ws);
    for (const auto& effect : effects_) {
        tmp.setVariable(effect.first, effect.second);
    }
	return tmp;
#endif
}

void CAction::Set_ActEffect(CWorldState& WorldState)
{
#ifdef _KJW_AI_CONTAINER_ARRAY
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
	{
		if (effects_[i] == 2)
			continue;

		WorldState.setVariable(i, effects_[i]);
	}
#else
	for (const auto& effect : effects_) {
		WorldState.setVariable(effect.first, effect.second);
	}
#endif
}

