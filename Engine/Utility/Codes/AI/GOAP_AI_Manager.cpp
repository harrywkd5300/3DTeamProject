
#include "AI\GOAP_AI_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CAI_Manager)

CAI_Manager::CAI_Manager() {

}

int CAI_Manager::calculateHeuristic(const CWorldState& now, const CWorldState& goal) const {
    return now.distanceTo(goal);
}

void CAI_Manager::addToOpenList(G_Node&& n) {
    // insert maintaining sort order
    auto it = lower_bound(begin(OpenList),
                               end(OpenList),
                               n);
    OpenList.emplace(it, move(n));
}

G_Node& CAI_Manager::popAndClose() {
    assert(!OpenList.empty());
    ClosedList.push_back(move(OpenList.front()));
    OpenList.erase(OpenList.begin());

    return ClosedList.back();
}

CAI_Manager::~CAI_Manager()
{
}

bool CAI_Manager::memberOfClosed(const CWorldState& ws) const {
    if (find_if(begin(ClosedList), end(ClosedList), [&](const G_Node & n) { return n.ws_ == ws; }) == end(ClosedList)) {
        return false;
    }
    return true;
}

vector<G_Node>::iterator CAI_Manager::memberOfOpen(const CWorldState& ws) {
    return find_if(begin(OpenList), end(OpenList), [&](const G_Node & n) { return n.ws_ == ws; });
}

void CAI_Manager::printOpenList() const {
    for (const auto& n : OpenList) {
        //cout << n << endl;
    }
}

void CAI_Manager::printClosedList() const {
    for (const auto& n : ClosedList) {
        //cout << n << endl;
    }
}

vector<CAction>* CAI_Manager::plan(const CWorldState& start, const CWorldState& goal, const vector<CAction>& actions, vector<CAction>* newPlan) {
    if (start.meetsGoal(goal)) {
        //throw runtime_error("CAI_Manager cannot plan when the start state and the goal state are the same!");
        return nullptr;
    }

    // Feasible we'd re-use a planner, so clear out the prior results
    OpenList.clear();
    ClosedList.clear();

    G_Node starting_node(start, 0, calculateHeuristic(start, goal), 0, nullptr);

    OpenList.push_back(move(starting_node));

    //int iters = 0;
    while (OpenList.size() > 0) {
        //++iters;
        //cout << "\n-----------------------\n";
        //cout << "Iteration " << iters << endl;

        // Look for G_Node with the lowest-F-score on the open list. Switch it to closed,
        // and hang onto it -- this is our latest node.
        G_Node& current(popAndClose());

        //cout << "Open list\n";
        //printOpenList();
        //cout << "Closed list\n";
        //printClosedList();
        //cout << "\nCurrent is " << current << " : " << (current.action_ == nullptr ? "" : current.action_->name()) << endl;

        // Is our current state the goal state? If so, we've found a path, yay.
        if (current.ws_.meetsGoal(goal)) {
            //vector<CAction> the_plan;
            do {
                newPlan->push_back(*current.action_);
                auto itr = find_if(begin(OpenList), end(OpenList), [&](const G_Node & n) { return n.id_ == current.parent_id_; });
                if (itr == end(OpenList)) {
                    itr = find_if(begin(ClosedList), end(ClosedList), [&](const G_Node & n) { return n.id_ == current.parent_id_; });
                }
                current = *itr;
            } while (current.parent_id_ != 0);
            return newPlan;
        }

        // Check each node REACHABLE from current -- in other words, where can we go from here?
        for (const auto& potential_action : actions) {
            if (potential_action.operableOn(current.ws_)) {
                CWorldState outcome = potential_action.actOn(current.ws_);

                // Skip if already closed
                if (memberOfClosed(outcome)) {
                    continue;
                }

                //cout << potential_action.name() << " will get us to " << outcome << endl;

                // Look for a G_Node with this CWorldState on the open list.
                auto p_outcome_node = memberOfOpen(outcome);
                if (p_outcome_node == end(OpenList)) { // not a member of open list
                    // Make a new node, with current as its parent, recording G & H
                    G_Node found(outcome, current.g_ + potential_action.cost(), calculateHeuristic(outcome, goal), current.id_, &potential_action);
                    // Add it to the open list (maintaining sort-order therein)
                    addToOpenList(move(found));
                } else { // already a member of the open list
                    // check if the current G is better than the recorded G
                    if (current.g_ + potential_action.cost() < p_outcome_node->g_) {
                        //cout << "My path to " << p_outcome_node->ws_ << " using " << potential_action.name() << " (combined cost " << current.g_ + potential_action.cost() << ") is better than existing (cost " <<  p_outcome_node->g_ << "\n";
                        p_outcome_node->parent_id_ = current.id_;                  // make current its parent
                        p_outcome_node->g_ = current.g_ + potential_action.cost(); // recalc G & H
                        p_outcome_node->h_ = calculateHeuristic(outcome, goal);
                        p_outcome_node->action_ = &potential_action;

                        // resort open list to account for the new F
                        // sorting likely invalidates the p_outcome_node iterator, but we don't need it anymore
                        sort(begin(OpenList), end(OpenList));
                    }
                }
            }
        }
    }	
	//vector<CAction> Failed;
	return nullptr;

    // If there's nothing left to evaluate, then we have no possible path left
    //throw runtime_error("A* planner could not find a path from start to goal");
}

CAction * CAI_Manager::plan(const CWorldState & start, const CWorldState & goal, const std::vector<CAction>& actions, CAction * NextAction)
{
	if (start.meetsGoal(goal)) {
		//throw runtime_error("CAI_Manager cannot plan when the start state and the goal state are the same!");
		return nullptr;
	}

	// Feasible we'd re-use a planner, so clear out the prior results
	OpenList.clear();
	ClosedList.clear();

	G_Node starting_node(start, 0, calculateHeuristic(start, goal), 0, nullptr);

	OpenList.push_back(move(starting_node));

	//int iters = 0;
	while (OpenList.size() > 0) {
		//++iters;
		//cout << "\n-----------------------\n";
		//cout << "Iteration " << iters << endl;

		// Look for G_Node with the lowest-F-score on the open list. Switch it to closed,
		// and hang onto it -- this is our latest node.
		G_Node& current(popAndClose());

		//cout << "Open list\n";
		//printOpenList();
		//cout << "Closed list\n";
		//printClosedList();
		//cout << "\nCurrent is " << current << " : " << (current.action_ == nullptr ? "" : current.action_->name()) << endl;

		// Is our current state the goal state? If so, we've found a path, yay.
		if (current.ws_.meetsGoal(goal)) {
			vector<CAction> the_plan;
			do {
				the_plan.push_back(*current.action_);
				auto itr = find_if(begin(OpenList), end(OpenList), [&](const G_Node & n) { return n.id_ == current.parent_id_; });
				if (itr == end(OpenList)) {
					itr = find_if(begin(ClosedList), end(ClosedList), [&](const G_Node & n) { return n.id_ == current.parent_id_; });
				}
				current = *itr;
			} while (current.parent_id_ != 0);
			*NextAction = (*the_plan.rbegin());
			return NextAction;
		}

		// Check each node REACHABLE from current -- in other words, where can we go from here?
		for (const auto& potential_action : actions) {
			if (potential_action.operableOn(current.ws_)) {
				CWorldState outcome = potential_action.actOn(current.ws_);

				// Skip if already closed
				if (memberOfClosed(outcome)) {
					continue;
				}

				//cout << potential_action.name() << " will get us to " << outcome << endl;

				// Look for a G_Node with this CWorldState on the open list.
				auto p_outcome_node = memberOfOpen(outcome);
				if (p_outcome_node == end(OpenList)) { // not a member of open list
													   // Make a new node, with current as its parent, recording G & H
					G_Node found(outcome, current.g_ + potential_action.cost(), calculateHeuristic(outcome, goal), current.id_, &potential_action);
					// Add it to the open list (maintaining sort-order therein)
					addToOpenList(move(found));
				}
				else { // already a member of the open list
					   // check if the current G is better than the recorded G
					if (current.g_ + potential_action.cost() < p_outcome_node->g_) {
						//cout << "My path to " << p_outcome_node->ws_ << " using " << potential_action.name() << " (combined cost " << current.g_ + potential_action.cost() << ") is better than existing (cost " <<  p_outcome_node->g_ << "\n";
						p_outcome_node->parent_id_ = current.id_;                  // make current its parent
						p_outcome_node->g_ = current.g_ + potential_action.cost(); // recalc G & H
						p_outcome_node->h_ = calculateHeuristic(outcome, goal);
						p_outcome_node->action_ = &potential_action;

						// resort open list to account for the new F
						// sorting likely invalidates the p_outcome_node iterator, but we don't need it anymore
						sort(begin(OpenList), end(OpenList));
					}
				}
			}
		}
	}
	//vector<CAction> Failed;
	return nullptr;
}

unsigned long CAI_Manager::Free(void)
{
	_ulong dwRefCnt = 0;


	return dwRefCnt;
}
