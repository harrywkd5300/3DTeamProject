/**
 * @class CAI_Manager
 * @brief Implements an A* algorithm for searching the action space
 *
 * @date July 2014
 * @copyright (c) 2014 Prylis Inc. All rights reserved.
 */

#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "AI\GOAP_Action.h"
#include "AI\GOAP_Node.h"
#include "AI\GOAP_WorldState.h"
// To support Google Test for private members
#ifndef TEST_FRIENDS
#define TEST_FRIENDS
#endif

BEGIN(Engine)

class DLL_EXPORT CAI_Manager : public CBase {
	DECLARE_SINGLETON(CAI_Manager)
private:
	explicit CAI_Manager();
	virtual ~CAI_Manager();
private:
	vector<G_Node> OpenList;   // The A* open list
	vector<G_Node> ClosedList; // The A* closed list
public:
	/**
	 Is the given worldstate a member of the closed list? (And by that we mean,
	 does any node on the closed list contain an equal worldstate.)
	 @param ws the worldstate in question
	 @return true if it's been closed, false if not
	 */
	bool memberOfClosed(const CWorldState& ws) const;

	/**
	 Is the given worldstate a member of the open list? (And by that we mean,
	 does any node on the open list contain an equal worldstate.)
	 @param ws the worldstate in question
	 @return a pointer to the note if found, end(OpenList) if not
	 */
	std::vector<G_Node>::iterator memberOfOpen(const CWorldState& ws);

	/**
	 Pops the first G_Node from the 'open' list, moves it to the 'closed' list, and
	 returns a reference to this newly-closed G_Node. Its behavior is undefined if
	 you call on an empty list.
	 @return a reference to the newly closed G_Node
	 */
	G_Node& popAndClose();

	/**
	 Moves the given G_Node (an rvalue reference) into the 'open' list.
	 @param an rvalue reference to a G_Node that will be moved to the open list
	 */
	void addToOpenList(G_Node&&);

	/**
	 Given two worldstates, calculates an estimated distance (the A* 'heuristic')
	 between the two.
	 @param now the present worldstate
	 @param goal the desired worldstate
	 @return an estimated distance between them
	 */
	int calculateHeuristic(const CWorldState& now, const CWorldState& goal) const;


	/**
	 Useful when you're debugging a GOAP plan: simply dumps the open list to stdout.
	*/
	void printOpenList() const;

	/**
	 Useful when you're debugging a GOAP plan: simply dumps the closed list to stdout.
	*/
	void printClosedList() const;

	/**
	 Actually attempt to formulate a plan to get from start to goal, given a pool of
	 available actions.
	 @param start the starting worldstate
	 @param goal the goal worldstate
	 @param actions the available action pool
	 @return a vector of Actions in REVERSE ORDER - use a reverse_iterator on this to get stepwise-order
	 @exception std::runtime_error if no plan could be made with the available actions and states
	 */
	vector<CAction>* plan(const CWorldState& start, const CWorldState& goal, const std::vector<CAction>& actions, vector<CAction>* newPlan);
	CAction* plan(const CWorldState& start, const CWorldState& goal, const std::vector<CAction>& actions, CAction* NextAction);
public:
	virtual unsigned long Free(void);


	TEST_FRIENDS;
};
END