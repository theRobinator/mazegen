#include "Node.h"
#include <iostream>

using namespace std;

Node::Node(int id) {
  this->id = id;
  parent = NULL;
  pathparent = NULL;
  painted = false;
  distance = -1;
}

Node::~Node() { }

Node *Node::getRoot() {
  if (parent == NULL) return this;
  else {
    Node *res = parent->getRoot();
    parent = res;
    return res;
  }
}

bool Node::hasEdge(int e) {
  std::list<Node *>::iterator it = edges.begin();
  for (; it != edges.end(); ++it) {
    if ((*it)->id == e) return true;
  }
  return false;
}
