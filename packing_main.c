#include "packing.h"
int main(int argc, char **argv) {
	char* inputFileName = NULL;
	char* outputFileName = NULL;
	Node *root;
	// Check if valid number of arguments is entered
	if(argc != 3) {
		fprintf(stderr, "Invalid number of arguments\n");
	} else {
		inputFileName = argv[1];
		outputFileName = argv[2];
	}
	double x=0,y=0,w,h;
	int max_label = -1;

	root = Load_From_File(inputFileName);

	Perform_Packing(root, &max_label, &x, &y);
	// Save to file frees the tree so store max's w and h
	w=root->value.width;
	h=root->value.height;

	Save_To_File(outputFileName, root);

	// Standard out dump
	printf("Width: %le\n", w);
	printf("Height: %le\n", h);
	printf("X-coordinate: %le\n", x);
	printf("Y-coordinate: %le", y);
	return 0;
}
