#ifndef PACKING_H
#define PACKING_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define INPUT_MAX_LENGTH 38

typedef struct _Package {
	char id;
	int label;
	double width;
	double height;
	double xcoord;
	double ycoord;
} Package;

typedef struct _Node {
	Package value;
	struct _Node *left;
	struct _Node *right;
} Node;


typedef struct _Stack {
	Node *node;
	struct _Stack *next;
} Stack;

Stack* makeStack();
Node *Load_From_File(char *Filename);
int Save_To_File(char *Filename, Node *root);
void Perform_Packing(Node *root, int *max_label, double *x, double *y);

#endif
