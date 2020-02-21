/* This file was automatically generated.  Do not edit! */
int main(void);
void orientEdge(struct Orientation *orientation,int src,int dest);
void deleteNode(struct Orientation *orientation,int src,int dest);
void printOrientation(struct Orientation *orientation);
void addEdge(struct Orientation *orientation,int src,int dest);
void appendNode(struct Node **list,struct Node *node,int v);
struct Orientation *createOrientation(int noV);
struct Node *createNode(int v);
