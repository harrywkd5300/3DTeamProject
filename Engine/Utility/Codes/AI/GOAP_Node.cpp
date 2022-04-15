#include "AI\GOAP_Node.h"
#include <iostream>

USING(Engine)
int G_Node::last_id_ = 0;

G_Node::G_Node() : g_(0), h_(0) {
    id_ = ++last_id_;
}
G_Node::G_Node(const CWorldState state, int g, int h, int parent_id, const CAction* action) :
    ws_(state), g_(g), h_(h), parent_id_(parent_id), action_(action) {
    id_ = ++last_id_;
}

bool operator<(const G_Node& lhs, const G_Node& rhs) {
    return lhs.f() < rhs.f();
}

bool G_Node::operator<(const G_Node& other) {
    return f() < other.f();
}
