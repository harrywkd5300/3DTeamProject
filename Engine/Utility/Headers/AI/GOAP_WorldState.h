/**
 * @class CWorldState
 * @brief A way of describing the "world" at any point in time.
 *
 * @date  July 2014
 * @copyright (c) 2014 Prylis Inc.. All rights reserved.
 */

#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)
    struct DLL_EXPORT CWorldState {
        float priority_; // useful if this is a goal state, to distinguish from other possible goals
        std::string name_; // the human-readable name of the state
#ifdef _KJW_AI_CONTAINER_MAP
		std::map<int, bool> vars_; // the variables that in aggregate describe a worldstate
#endif
#ifdef _KJW_AI_CONTAINER_UNORDED_MAP
		std::unordered_map<int, bool> vars_; // the variables that in aggregate describe a worldstate
#endif
#ifdef _KJW_AI_CONTAINER_VECTOR
		std::vector<bool> vars_; // the variables that in aggregate describe a worldstate
#endif
#ifdef _KJW_AI_CONTAINER_ARRAY
		//bool vars_[AI_ARRAY_SIZE_MAX] = {false,};
		byte vars_[AI_ARRAY_SIZE_MAX];
		
		int vars_Size = 0;
		int vars_Max_Number = 0;
		CWorldState(const CWorldState& rhs);
		//CWorldState();
#endif
		
        CWorldState(const std::string name="");

        /**
         Set a world state variable, e.g. "gunLoaded" / true
         @param var_id the unique ID of the state variable
         @param value the boolean value of the variable
         */
	public:
        void setVariable(const int var_id, const bool value);
		void setVariable(const int var_Start_id, const int var_End_id, const bool value);

		_int GetCurAniNum(const _int StartAniNum, const _int EndAniNum)
		{
#ifdef _KJW_AI_CONTAINER_ARRAY
			for (int i = StartAniNum; i < EndAniNum; ++i)
			{
				if (vars_[i] == 1)
					return i;
			}
			return -1;
#else
			for (int i = StartAniNum; i < EndAniNum; ++i)
			{
				if (vars_[i] == true)
					return i;
			}
			return -1;
#endif
		}

        /**
         Retrieve the current value of the given variable.
         @param var_id the unique ID of the state variable
         @return the value of the variable
        */
        bool getVariable(const int var_id) const;

        /**
         Useful if this state is a goal state. It asks, does state 'other'
         meet the requirements of this goal? Takes into account not only this goal's
         state variables, but which variables matter to this goal state.
         @param other the state you are testing as having met this goal state
         @return true if it meets this goal state, false otherwise
         */
        bool meetsGoal(const CWorldState& goal_state) const;

        /**
         Given the other state -- and what 'matters' to the other state -- how many
         of our state variables differ from the other?
         @param other the goal state to compare against
         @return the number of state-var differences between us and them
         */
        int distanceTo(const CWorldState& goal_state) const;

        /**
         Equality operator
         @param other the other worldstate to compare to
         @return true if they are equal, false if not
         */
        bool operator==(const CWorldState& other) const;

        // A friend function of a class is defined outside that class' scope but it has the
        // right to access all private and protected members of the class. Even though the
        // prototypes for friend functions appear in the class definition, friends are not
        // member functions.
        //friend std::ostream& operator<<(std::ostream& out, const CWorldState& n);
    };

    //inline std::ostream& operator<<(std::ostream& out, const CWorldState& n) {
    //    out << "CWorldState { ";
    //    for (const auto& kv : n.vars_) {
    //        out << kv.second << " ";
    //    }
    //    out << "}";
    //    return out;
    //}

END