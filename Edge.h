#ifndef __EDGE_H__
#define __EDGE_H__

#include "Node.h"

class Edge {
 public:
  Node *n1, *n2;
  int id, weight;

  Edge(Node *left, Node *right, int id);
  Edge(Node *left, Node *right, int id, int weight);
};

#endif
