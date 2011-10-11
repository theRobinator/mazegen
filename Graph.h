#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "Node.h"
#include "Edge.h"
#include <iostream>
#include <vector>
#include <list>

using namespace std;

/*
 * This class represents a graph that keeps track of
 * its own connected components.
 */
class Graph {
 public:
  int width, height;
  Node **nodes;
  std::vector<Edge *> edges;

  Graph(int w, int h);
  Graph(int w, int h, std::vector<Node*> ns, std::vector<Edge*> es);
  ~Graph();

  bool connected(Node *n1, Node *n2);
  Edge *removeEdge(int e, bool info);
  int *BFSpaint(Node *start, Node *finish);
  void setEdgeWeight(int n1, int n2, int weight);
  int dijkstraPaint(Node *start, Node *finish);
  
 private:
  Edge *edgeFor(int n1, int n2);
  Node *getMinDistance(std::list<Node*> queue);
};

#endif
