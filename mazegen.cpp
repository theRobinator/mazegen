#include <iostream>
#include <fstream>
#include <string>
#include "Graph.h"

using namespace std;

bool generating = false, info = false, type2 = false;
int width, height;
int top, bottom, leftedge, rightedge;
istream *in = &cin;

bool startsWith(char *s1, string s) {
  const char *s2 = s.c_str();
  for (int i = 0; s2[i]; ++i) {
    if (s1[i] != s2[i]) return false;
  }
  return true;
}

void usageExit() {
  cerr << "Usage:" << endl;
  cerr << "mazegen gen w h [-info] [-top=tpos] [-bottom=bpos] [-left=lpos] [-right=rpos] [rndfile]" << endl;
  cerr << "mazegen solve [file]" << endl;
  exit(1);
}

//This function parses options AFTER the mode has been specified.
void parseOptions(int argc, char *argv[]) {
  int specified = 0;
  top = bottom = leftedge = rightedge = -1;
  if (generating) {
    if (argc < 4) usageExit();
    width = atoi(argv[2]);
    height = atoi(argv[3]);
    if (width <= 0 || height <= 0) {
      cerr << "You may only specify positive, non-zero dimensions.\n";
      exit(1);
    }
    for (int i = 4; i < argc; ++i) {
      if (strcmp(argv[i],"-info") == 0) {
	info = true;
      }else if (startsWith(argv[i],"-top=")) {
	top = atoi(argv[i]+5); //pointer arithmetic for substring
	specified++;
      }else if (startsWith(argv[i],"-bottom=")) {
	bottom = atoi(argv[i]+8);
	specified++;
      }else if (startsWith(argv[i],"-left=")) {
	leftedge = atoi(argv[i]+6);
	specified++;
      }else if (startsWith(argv[i],"-right=")) {
	rightedge = atoi(argv[i]+7);
	specified++;
      }else if (argv[i][0] == '-') {
	cerr << "Unrecognized option: " << argv[i] << endl;
	exit(1);
      }else if (i == argc-1) { //The last argument, randfile
	in = new ifstream(argv[i]);
      }else {
	usageExit();
      }
    }
    if (specified != 0 && specified != 2) {
      cerr << "You must specify either 2 or 0 coordinates for the starting position." << endl;
      exit(1);
    }else if (specified == 0) {
      top = 0;
      bottom = width-1;
    }else { // 2 specified; convert to 0-based
      --top;
      --bottom;
      --leftedge;
      --rightedge;
    }
    
  //The below is for solving mode
  }else {
    if (argc == 3) {
      if (argv[2][0] == '-') {
	cerr << "Unrecognized option: " << argv[2] << endl;
	exit(1);
      }
      in = new ifstream(argv[2]);
    } else if (argc > 3)
      usageExit();
  }
}

unsigned int bytes2int(char *buf) {
  unsigned int res = 0;
  res |= ((unsigned int)buf[0] & 0xFF) << 24;
  res |= ((unsigned int)buf[1] & 0xFF) << 16;
  res |= ((unsigned int)buf[2] & 0xFF) << 8;
  res |= ((unsigned int)buf[3] & 0xFF);
  return res;
}

void formatExit() {
  cerr << "Improperly formatted maze file.\n";
  exit(1);
}

Graph *readMaze() {
  char *hline = new char[1024], *vline = new char[1024];
  int h = height = width = 0;
  std::vector<Node*> toadd;
  std::vector<Edge*> edgeadd;
  Node *tmp1, *tmp2;

  while ((*in).getline(hline,1024)) {
    (*in).getline(vline,1024);
    if (width == 0) width = (strlen(hline)-1) / 2;
    else if ((int)(strlen(hline)-1) / 2 != width)
      formatExit();

    for (int i = 0; i < width; ++i) toadd.push_back(new Node(width*h+i));

    if (strlen(vline) == 0) {
      if (hline[0] != '+') formatExit();
      for (int w = 1; hline[w]; ++w) {
        if (hline[w] == '+' || hline[w] == '-') continue;
        else if (hline[w] == ' ') {
          if (bottom < 0) bottom = w / 2;
          else formatExit();
        }
      }
      break;
    }else if (hline[0] == '+') {
      for (int i = 0; i < width; ++i) {
        int j = 2*i+1;
        if (hline[j+1] != '+') formatExit();
        else if (hline[j] == '-') continue;
        else if (hline[j] == ' ') {
          if (h == 0)
            if (top < 0) top = i;
            else formatExit();
          else {
            toadd[width*h+i]->edges.push_back(toadd[width*(h-1)+i]);
            toadd[width*(h-1)+i]->edges.push_back(toadd[width*h+i]);
          }
        }else if (hline[j] > 47 && hline[j] < 58) { //A number
          type2 = true;
	  tmp1 = toadd[width*h+i];
	  tmp2 = toadd[width*(h-1)+i];
	  edgeadd.push_back(new Edge(tmp1, tmp2, -1, (int)(hline[j]-48)));
	  tmp1->edges.push_back(tmp2);
	  tmp2->edges.push_back(tmp1);
        }else {
          formatExit();
        }
      }
    }else formatExit();

    if (vline[0] == '|' || vline[0] == ' ') {
      if ((int)(strlen(vline)-1)/2 != width) formatExit();
      if (vline[0] == ' ')
        if (leftedge < 0) leftedge = h;
        else formatExit();
      for (int i = 0; i < width; ++i) {
        int j = 2*i+1;
        if (vline[j] != ' ') formatExit();
        else if (vline[j+1] == '|') continue;
        else if (vline[j+1] == ' ') {
          if (i == width-1)
            if (rightedge < 0) rightedge = h;
            else formatExit();
          else {
            toadd[width*h+i]->edges.push_back(toadd[width*h+i+1]);
            toadd[width*h+i+1]->edges.push_back(toadd[width*h+i]);
          }
        }else if (vline[j+1] > 47 && vline[j+1] < 58) {
          type2 = true;
	  tmp1 = toadd[width*h+i];
	  tmp2 = toadd[width*h+i+1];
	  edgeadd.push_back(new Edge(tmp1,tmp2,-1,(int)(vline[j+1]-48)));
	  tmp1->edges.push_back(tmp2);
	  tmp2->edges.push_back(tmp1);
        }
      }
      ++h;
    }else {
      formatExit();
    }
  }
  height = h;
  Graph *g = new Graph(width, height, toadd, edgeadd);
  delete &toadd;
  return g;
}

void printMaze(Graph *g) {
  if (top >= 0) {
    g->nodes[top]->edges.push_back(new Node(top-width));
  }
  for (int h = 0; h < height; ++h) {
    cout << "+";
    for (int i = 0; i < width; ++i) {
      if (g->nodes[width*h+i]->hasEdge(width*(h-1)+i)) {
	if (g->nodes[width*h+i]->painted &&
	    ((width*(h-1)+i >= 0 && g->nodes[width*(h-1)+i]->painted)
	    || (h == 0 && top == i)))
	  cout << "@";
	else
	  cout << " ";
      }else {
	cout << "-";
      }
      cout << "+";
    }
    cout << "\n";
    if (leftedge == h) {
      if (generating)
	cout << " ";
      else
	cout << "@";
    }
    else cout << "|";
    for (int i = 0; i < width; ++i) {
      if (g->nodes[width*h+i]->painted)
	cout << "@";
      else
	cout << " ";
      if (g->nodes[width*h+i]->hasEdge(width*h+i+1) ||
	  (rightedge == h && i == width-1)) {
	if (rightedge == h && i == width-1 && !generating)
	  cout << "@";
	else if (g->nodes[width*h+i]->painted &&
		 (width*h+i+1 < width*height && g->nodes[width*h+i+1]->painted))
	  cout << "@";
	else
	  cout << " ";
      }else {
	cout << "|";
      }
    }
    cout << "\n";
  }
  cout << "+";
  for (int w = 0; w < width; ++w) {
    if (bottom == w)
      if (g->nodes[width*(height-1)+w]->painted)
	cout << "@";
      else
	cout << " ";
    else
      cout << "-";
    cout << "+";
  }
  cout << "\n";
}

string printCross(Graph *g, int topleft) {
  if (topleft < 0) {
    if (topleft == -1*width-1) return "<img src=\"imgs/0110.gif\">";
    if (topleft % width == -1) return "<img src=\"imgs/0011.gif\">";
    if (!g->nodes[topleft+width]->hasEdge(topleft+width+1)) return "<img src=\"imgs/0111.gif\">";
    return "<img src=\"imgs/0101.gif\">";
  }
  if (topleft / width == height - 1) {
    if (topleft == width*height-1) return "<img src=\"imgs/1001.gif\">";
    if (!g->nodes[topleft]->hasEdge(topleft+1)) return "<img src=\"imgs/1101.gif\">";
    return "<img src=\"imgs/0101.gif\">";
  }
  if (topleft % width == width-1) {
    if (!g->nodes[topleft]->hasEdge(topleft+width)) return "<img src=\"imgs/1011.gif\">";
    return "<img src=\"imgs/1010.gif\">";
  }
  char checks[] = {'0','0','0','0','\0'};
  if (!g->nodes[topleft]->hasEdge(topleft+1)) {
    checks[0] = '1';
  }
  if (!g->nodes[topleft+1]->hasEdge(topleft+1+width)) {
    checks[1] = '1';
  }
  if (!g->nodes[topleft+width]->hasEdge(topleft+1+width)) {
    checks[2] = '1';
  }
  if (!g->nodes[topleft]->hasEdge(topleft+width)) {
    checks[3] = '1';
  }
  return "<img src=\"imgs/"+string(checks)+".gif\">";
}

void printHTML(Graph *g) {
  cout << "<html><head><title>MAZE</title></head><body style=\"overflow: scroll; white-space:nowrap;\">" << endl;
  if (top >= 0) {
    g->nodes[top]->edges.push_back(new Node(top-width));
  }
  for (int h = 0; h < height; ++h) {
    if (h == 0) cout << "<img src=\"imgs/0110.gif\">";
    else if (!g->nodes[width*(h-1)]->hasEdge(width*h)) cout << "<img src=\"imgs/1110.gif\">";
    else cout << "<img src=\"imgs/1010.gif\">";
    cout.flush();
    for (int i = 0; i < width; ++i) {
      if (g->nodes[width*h+i]->hasEdge(width*(h-1)+i)) {
        if (g->nodes[width*h+i]->painted &&
            ((width*(h-1)+i >= 0 && g->nodes[width*(h-1)+i]->painted)
             || (h == 0 && top == i)))
          cout << "@";
        else
          cout << "<img src=\"imgs/0000.gif\">";
      }else {
        cout << "<img src=\"imgs/0101.gif\">";
      }
      cout << printCross(g, width*h+i-width);
    }
    cout << "<br>";
    if (leftedge == h) {
      if (generating)
        cout << "<img src=\"imgs/0000.gif\">";
      else
        cout << "@";
    }
    else cout << "<img src=\"imgs/1010.gif\">";
    for (int i = 0; i < width; ++i) {
      if (g->nodes[width*h+i]->painted)
        cout << "@";
      else
        cout << "<img src=\"imgs/0000.gif\">";
      if (g->nodes[width*h+i]->hasEdge(width*h+i+1) ||
          (rightedge == h && i == width-1)) {
        if (rightedge == h && i == width-1 && !generating)
          cout << "@";
        else if (g->nodes[width*h+i]->painted &&
                 (width*h+i+1 < width*height && g->nodes[width*h+i+1]->painted))
          cout << "@";
        else
          cout << "<img src=\"imgs/0000.gif\">";
      }else {
        cout << "<img src=\"imgs/1010.gif\">";
      }
    }
    cout << "<br>";
  }
  cout << "<img src=\"imgs/1100.gif\">";
  for (int w = 0; w < width; ++w) {
    if (bottom == w)
      if (g->nodes[width*(height-1)+w]->painted)
        cout << "@";
      else
        cout << "<img src=\"imgs/0000.gif\">";
    else
      cout << "<img src=\"imgs/0101.gif\">";
    cout << printCross(g, width*(height-1)+w);
  }
  cout << "<br></body></html>\n";
}

void solve() {
  Graph *g = readMaze();
  Node **nodes = new Node*[2];
  int pos = 0;
  if (top >= 0) {
    nodes[pos] = g->nodes[top];
    ++pos;
  }
  if (bottom >= 0) {
    nodes[pos] = g->nodes[width*(height-1)+bottom];
    ++pos;
  }
  if (leftedge >= 0) {
    if (pos > 1) formatExit();
    nodes[pos] = g->nodes[leftedge*width];
    ++pos;
  }
  if (rightedge >= 0) {
    if (pos > 1) formatExit();
    nodes[pos] = g->nodes[rightedge*width+width-1];
    ++pos;
  }
  if (nodes[0]->id > nodes[1]->id) {
    Node *tmp = nodes[1];
    nodes[1] = nodes[0];
    nodes[0] = tmp;
  }
  if (pos <= 1) formatExit(); //Not enough exits
  if (type2) {
    int info = g->dijkstraPaint(nodes[0], nodes[1]);
    if (info == -1) formatExit();
    printMaze(g);
    cout << "Starts in room at row = " << (nodes[0]->id / width)+1 << " and column = " << (nodes[0]->id % width)+1 << ".\n";
    cout << "Ends in room at row = " << (nodes[1]->id / width)+1 << " and column = " << (nodes[1]->id % width)+1 << ".\n";
    cout << "Cost of shortest path = " << info << ".\n";
  }else {
    int *info = g->BFSpaint(nodes[0],nodes[1]);
    if (info[0] == -1) formatExit();
    printMaze(g);
    cout << "Starts in room at row = " << (nodes[0]->id / width)+1 << " and column = " << (nodes[0]->id % width)+1 << ".\n";
    cout << "Ends in room at row = " << (nodes[1]->id / width)+1 << " and column = " << (nodes[1]->id % width)+1 << ".\n";
    cout << "Cost of shortest path = " << info[0] << ".\n";
    cout << "Maximum level = " << info[1] << ".\n";
  }
}

void generate() {
  char *buf = new char[4];
  Graph *graph = new Graph(width, height);
  int connectedrooms = 0;

  while (true) {
    (*in).read(buf,4);
    if (in->eof()) {
      cerr << "Not enough random data to generate a complete maze..." << endl;
      exit(1);
    }

    unsigned int i = bytes2int(buf);
    Edge *e = graph->removeEdge(i % graph->edges.size(), info);
    if (!graph->connected(e->n1, e->n2)) {
      if (info) cout << "Wall " << e->id << " removed.\n";
      e->n1->edges.push_back(e->n2);
      e->n2->edges.push_back(e->n1);
      e->n1->getRoot()->parent = e->n2;
      connectedrooms++;
      if (connectedrooms == width*height-1) break;
    }
  }

  printHTML(graph);
}

int main(int argc, char *argv[]) {
  if (argc == 1) usageExit();
  if (strcmp(argv[1],"gen") == 0)
    generating = true;
  else if (strcmp(argv[1],"solve") == 0)
    generating = false;
  else
    usageExit();

  parseOptions(argc, argv);
  
  if (in->fail()) {
    cerr << "Could not read from file!" << endl;
    exit(1);
  }

  if (generating) {
    generate();
  }else {
    solve();
  }
}
