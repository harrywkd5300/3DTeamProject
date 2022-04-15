/**
 * @class CAction
 * @brief Operates on the world state.
 *
 * @date  July 2014
 * @copyright (c) 2014 Prylis Inc.. All rights reserved.
 */

#pragma once

#include "Engine_Defines.h"
#include "Base.h"

 // To support Google Test for private members
#ifndef TEST_FRIENDS
#define TEST_FRIENDS
#endif

BEGIN(Engine)
struct CWorldState;

class DLL_EXPORT CAction {
private:
	std::string name_; // The human-readable action name
	int cost_;         // The numeric cost of this action
	// Preconditions are things that must be satisfied before this
#ifdef _KJW_AI_CONTAINER_ARRAY
	byte preconditions_[AI_ARRAY_SIZE_MAX] = { 2 };
	int preconditions_Size = 0;
	int preconditions_Max_Number = 0;

	byte effects_[AI_ARRAY_SIZE_MAX] = { 2 };
	int effects_Size = 0;
	int effects_Max_Number = 0;
#else
	// action can be taken. Only preconditions that "matter" are here.
	std::unordered_map<int, bool> preconditions_;

	// Effects are things that happen when this action takes place.
	std::unordered_map<int, bool> effects_;
#endif
public:
#ifdef _KJW_AI_CONTAINER_ARRAY
	CAction(const CAction& rhs);
#endif
	CAction();
	CAction(std::string name, int cost);

	/**
	 Is this action eligible to operate on the given worldstate?
	 @param ws the worldstate in question
	 @return true if this worldstate meets the preconditions
	 */
	bool operableOn(const CWorldState& ws) const;

	/**
	 Act on the given worldstate. Will not check for "eligiblity" and will happily
	 act on whatever worldstate you provide it.
	 @param the worldstate to act on
	 @return a copy worldstate, with effects applied
	 */
	CWorldState actOn(const CWorldState& ws) const;
	void Set_ActEffect(CWorldState& WorldStatem);
	/**
	 Set the given precondition variable and value.
	 @param key the name of the precondition
	 @param value the value the precondition must hold
	 */
	void setPrecondition(const int key, const bool value) {
#ifdef _KJW_AI_CONTAINER_ARRAY
		if (preconditions_[key] == 2)
		{
			if (preconditions_Max_Number < key)
				preconditions_Max_Number = key;
			++preconditions_Size;
		}
		preconditions_[key] = value;
#else
		preconditions_[key] = value;
#endif
	}
	void setPrecondition(const int StartAnikey, const int EndAnikey, const bool value = false) {
#ifdef _KJW_AI_CONTAINER_ARRAY
		for (int i = StartAnikey + 1; i < EndAnikey; ++i)
		{
			if (preconditions_[i] == 2)
			{
				if (preconditions_Max_Number < i)
					preconditions_Max_Number = i;
				++preconditions_Size;
			}
			preconditions_[i] = value;
		}
#else
		for (int i = StartAnikey + 1; i < EndAnikey; ++i)
		{
			preconditions_[i] = value;
		}
#endif
	}
	/**
	 Set the given effect of this action, in terms of variable and new value.
	 @param key the name of the effect
	 @param value the value that will result
	 */
	void setEffect(const int key, const bool value) {
#ifdef _KJW_AI_CONTAINER_ARRAY
		if (effects_[key] == 2)
		{
			if (effects_Max_Number < key)
				effects_Max_Number = key;
			++effects_Size;
		}
		effects_[key] = value;
#else
		effects_[key] = value;
#endif

	}



	void SetEffect_Arr(const int StartAnikey, const int EndAnikey, const bool value = false) {
#ifdef _KJW_AI_CONTAINER_ARRAY
		for (int i = StartAnikey + 1; i < EndAnikey; ++i)
		{
			if (effects_[i] == 2)
			{
				if (effects_Max_Number < i)
					effects_Max_Number = i;
				++effects_Size;
			}
			effects_[i] = value;
		}
#else
		for (int i = StartAnikey + 1; i < EndAnikey; ++i)
		{
			effects_[i] = value;
		}
#endif
	}

	void Set_Name(char* name) { name_ = name; }
	void Set_Cost(int cost) { cost_ = cost; }

	int Get_Animation(const int StartAnikey, const int EndAnikey) {

#ifdef _KJW_AI_CONTAINER_ARRAY
		for (int i = StartAnikey + 1; i < EndAnikey; ++i)
		{
			if (effects_[i] == 1)
				return i;
		}
		return 0;
#else
		for (int i = StartAnikey + 1; i < EndAnikey; ++i)
		{
			if (effects_[i] == true)
				return i;
		}
		return 0;
#endif
	}
	//int Set_Animation(const int StartAnikey, const int EndAnikey) {
	//	for (int i = StartAnikey + 1; i < EndAnikey; ++i)
	//	{
	//		if (effects_[i] == true)
	//			return i;
	//	}
	//	return 0;
	//}

	int cost() const { return cost_; }

	std::string name() const { return name_; }

	void Reset_Effect(const int key)
	{
		if (effects_[key] == 2)
			return;
		effects_[key] = 2;
		--effects_Size;
		effects_Max_Number = 0;
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			if (effects_[i] != 2)
			{
				if (effects_Max_Number < i)
					effects_Max_Number = i;
			}
		}
	};
	void Reset_Precondition(const int key)
	{
		if (preconditions_[key] == 2)
			return;
		preconditions_[key] = 2;
		--preconditions_Size;
		preconditions_Max_Number = 0;
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			if (preconditions_[i] != 2)
			{
				if (preconditions_Max_Number < i)
					preconditions_Max_Number = i;
			}
		}
	};
	void SetName(string newName) { name_ = newName; }
	void SetCost(int iCost) { cost_ = iCost; }
	void Reset()
	{
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			preconditions_[i] = 2;
			effects_[i] = 2;
		}

		preconditions_Size = 0;
		preconditions_Max_Number = 0;
		effects_Size = 0;
		effects_Max_Number = 0;
		name_ = "";
		cost_ = 0;
	};
	byte* Get_Precondition_Arr_ForMapTool() { return preconditions_; }
	byte* Get_Effect_Arr_ForMapTool() { return effects_; }
	int Get_PreSize() { return preconditions_Size; }
	int Get_PreMaxNum() { return preconditions_Max_Number; }
	int Get_EffectSize() { return effects_Size; }
	int Get_EffectMaxNum() { return effects_Max_Number; }
	
	TEST_FRIENDS;
};

END