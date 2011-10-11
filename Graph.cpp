#include <queue>
#include "Graph.h"

Graph::Graph(int w, int h) {
  width = w;
  height = h;
  int numnodes = w*h;
  nodes = new Node*[numnodes];


  // Add all the nodes
  for (int i = 0; i < numnodes; ++i) {
    nodes[i] = new Node(i);
  }

  int ecounter = 0;

  // Add all the edges
  for (int i = 1; i < width; ++i) {
    edges.push_back(new Edge(nodes[i],nodes[i-1], ecounter));
    ecounter++;
  }
  for (int offset = width; offset < numnodes; offset += width) {
    for (int i = 0; i < width; ++i) {
      edges.push_back(new Edge(nodes[offset+i],nodes[offset+i-w], ecounter));
      ecounter++;
    }
    for (int i = 1; i < width; ++i) {
      edges.push_back(new Edge(nodes[offset+i],nodes[offset+i-1], ecounter));
      ecounter++;
    }
  }
}

Graph::Graph(int w, int h, std::vector<Node*> ns, std::vector<Edge*> es) {
  width = w;
  height = h;
  int numnodes = w*h;
  nodes = new Node*[numnodes];


  // Add all the nodes
  for (int i = 0; i < numnodes; ++i) {
    nodes[i] = ns[i];
  }

  int ecounter = 0;

  // Add all the edges
  for (int i = 1; i < width; ++i) {
    edges.push_back(new Edge(nodes[i],nodes[i-1], ecounter));
    ecounter++;
  }
  for (int offset = width; offset < numnodes; offset += width) {
    for (int i = 0; i < width; ++i) {
      edges.push_back(new Edge(nodes[offset+i],nodes[offset+i-w], ecounter));
      ecounter++;
    }
    for (int i = 1; i < width; ++i) {
      edges.push_back(new Edge(nodes[offset+i],nodes[offset+i-1], ecounter));
      ecounter++;
    }
  }

  //Set edge weights
  for (std::vector<Edge*>::iterator it = es.begin(); it != es.end(); ++it) {
    edgeFor((*it)->n1->id,(*it)->n2->id)->weight = (*it)->weight;
  }
}

Graph::~Graph() {
  for (int i = 0; i < width*height; ++i) {
    delete &nodes[i];
  }
  edges.clear();
}

bool Graph::connected(Node *n1, Node *n2) {
  return (n1->getRoot() == n2->getRoot());
}

Edge *Graph::removeEdge(int edge, bool info) {
  std::vector<Edge *>::iterator it = edges.begin();
  it += edge;
  Edge *e = *it;
  edges.erase(it);
  return e;
}

int *Graph::BFSpaint(Node *start, Node *finish) {
  int *res = new int[2];
  res[0] = res[1] = 0;
  start->pathparent = NULL;
  std::list<Node*> queue;
  start->distance = 0;
  queue.push_front(start);
  Node *curr, *tmp;
  std::list<Node*>::iterator it;
  while (!queue.empty()) {
    curr = queue.front();
    queue.pop_front();
    if (curr->distance > res[1]) res[1] = curr->distance;
    if (curr == finish && res[0] == 0) {
      tmp = curr;
      while (curr != NULL) {
	curr->painted = true;
	curr = curr->pathparent;
      }
      curr = tmp;
    }
    for (it = curr->edges.begin(); it != curr->edges.end(); ++it) {
      if ((*it)->distance != -1) continue;
      (*it)->pathparent = curr;
      (*it)->distance = curr->distance + 1;
      queue.push_back(*it);
    }
  }
  res[0] = finish->distance;
  return res;
}

void Graph::setEdgeWeight(int n1, int n2, int weight) {
  Edge *e = edgeFor(n1,n2);
  e->weight = weight;
}

Edge *Graph::edgeFor(int n1, int n2) {
  int small = n1, large = n2;
  if (n1 > n2) {
    small = n2;
    large = n1;
  }
  if (large - small == 1) { //vertical edge
    return edges[small+(width-1)*(small / width)];
  }else {
    return edges[small+(width-1)*(small / width + 1)];
  }
}

Node * Graph::getMinDistance(std::list<Node*> queue) {
  int min = 0xFFFF; //Max value on a 32-bit system
  Node *res = NULL;
  std::list<Node*>::iterator it, toremove;
  for (it = queue.begin(); it != queue.end(); ++it) {
    if ((*it)->distance <= min) {
      min = (*it)->distance;
      res = *it;
      toremove = it;
    }
  }
  return res;
}

int Graph::dijkstraPaint(Node *start, Node *end) {
  start->distance = 0;
  std::list<Node*> q;
  std::list<Node*>::iterator it;
  Node *curr;
  q.push_front(start);
  while (!q.empty()) {
    curr = getMinDistance(q);
    q.remove(curr);
    if (curr == end) {
      int totaldist = curr->distance;
      while (curr != NULL) {
	curr->painted = true;
	curr = curr->pathparent;
      }
      return totaldist;
    }
    for (it = curr->edges.begin(); it != curr->edges.end(); ++it) {
      int newpi = curr->distance + (edgeFor(curr->id,(*it)->id))->weight;
      if ((*it)->distance == -1) {
	(*it)->pathparent = curr;
	(*it)->distance = newpi;
	q.push_back(*it);
      }else if (newpi < (*it)->distance) {
	(*it)->pathparent = curr;
	(*it)->distance = newpi;
      }
    }
  }
  return -1;
}
