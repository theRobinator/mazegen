#ifndef __NODE_H__
#define __NODE_H__

#include <list>

/*
 * This class represents a node in a graph.
 */
class Node {
 public:
  int id, distance;
  bool painted;
  Node *parent, *pathparent;
  std::list<Node*> edges;

  Node(int id);
  ~Node();

  Node *getRoot();
  bool hasEdge(int e);
};

#endif
