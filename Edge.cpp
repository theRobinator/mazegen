#include <iostream>
#include "Edge.h"
using namespace std;

Edge::Edge(Node *left, Node *right, int id) {
  n1 = left;
  n2 = right;
  this->id = id;
  weight = 1;
}

Edge::Edge(Node *left, Node *right, int id, int weight) {
  n1 = left;
  n2 = right;
  this->id = id;
  this->weight = weight;
}
