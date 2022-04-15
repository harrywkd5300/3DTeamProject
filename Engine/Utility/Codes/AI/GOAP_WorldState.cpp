#include "AI\GOAP_WorldState.h"

USING(Engine)

#ifdef _KJW_AI_CONTAINER_ARRAY
CWorldState::CWorldState(const CWorldState & rhs)
{ 

	//memcpy(vars_, rhs.vars_, sizeof(_byte)*AI_ARRAY_SIZE_MAX);
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		vars_[i] = rhs.vars_[i];
	
	name_ = rhs.name_;
	priority_ = rhs.priority_;
	vars_Size = rhs.vars_Size;
	vars_Max_Number = rhs.vars_Max_Number;
}
#endif
CWorldState::CWorldState(const std::string name) : priority_(0), name_(name) {
#ifdef _KJW_AI_CONTAINER_ARRAY
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		vars_[i] = 2;
#endif

}

void CWorldState::setVariable(const int var_id, const bool value) {
#ifdef _KJW_AI_CONTAINER_ARRAY
	if (vars_[var_id] == 2)
	{
		if (vars_Max_Number < var_id)
			vars_Max_Number = var_id;
		++vars_Size;
	}
    vars_[var_id] = value;
#else
	vars_[var_id] = value;
#endif
}

void CWorldState::setVariable(const int var_Start_id, const int var_End_id, const bool value)
{
#ifdef _KJW_AI_CONTAINER_ARRAY
	for (int i = var_Start_id; i <= var_End_id; ++i)
	{
		if (vars_[i] == 2)
		{
			if (vars_Max_Number < i)
				vars_Max_Number = i;
			++vars_Size;
		}
		vars_[i] = value;
	}
#else
	for (int i = var_Start_id + 1; i < var_End_id; ++i)
	{
		vars_[i] = value;
	}
#endif
}

bool CWorldState::getVariable(const int var_id) const {
#ifdef _KJW_AI_CONTAINER_ARRAY
	return vars_[var_id];
#else
    return vars_.at(var_id);
#endif
}


bool CWorldState::operator==(const CWorldState& other) const {
#ifdef _KJW_AI_CONTAINER_ARRAY
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
	{
		if (other.vars_[i] != vars_[i])
			return false;
	}
	return true;
#else
    return (vars_ == other.vars_);
#endif
}

bool CWorldState::meetsGoal(const CWorldState& goal_state) const {
#ifdef _KJW_AI_CONTAINER_ARRAY
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i) {
		if(goal_state.vars_[i]==2)
			continue;

		if (vars_[i] != goal_state.vars_[i]) {
			return false;
		}
	}
	return true;
#else
	for (const auto& kv : goal_state.vars_) {
		try {
			if (vars_.at(kv.first) != kv.second) {
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

int CWorldState::distanceTo(const CWorldState& goal_state) const {
#ifdef _KJW_AI_CONTAINER_ARRAY
	int result = 0;
	for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i) 
	{
		if (goal_state.vars_[i] == 2)
			continue;

		if (i == vars_Max_Number || vars_[i] != goal_state.vars_[i]) {
			++result;
		}
	}
	
	return result;
#else
    int result = 0;
    for (const auto& kv : goal_state.vars_) {
        auto itr = vars_.find(kv.first);
        if (itr == end(vars_) || itr->second != kv.second) {
            ++result;
        }
    }

    return result;
#endif
}