#include "packing.h"

// Stack Functions (Implemented with linked list):
// Initializes stack
Stack* makeStack() {
	Stack* newStack = malloc(sizeof(Stack));
	newStack->node = NULL;
	newStack->next = NULL;
	return newStack;
}

// Get item at top
Node* top(Stack *s) {
	if(s->next == NULL) {
		fprintf(stderr, "Top of stack is empty\n");
		return NULL;
	}
	return s->next->node;
}

// Pushes onto stack
void push(Stack *s, Node* node) {
	Stack* newStack = malloc(sizeof(Stack));
	Stack* followUp = s->next;
	newStack->node = node;
	newStack->next = followUp;
	s->next = newStack;
}

// Removes item and frees stack node. Node from stack is returned
Node* pop(Stack *s) {
	if(s->next == NULL) {
		fprintf(stderr, "pop(Stack *s) attempted to pop empty stack\n");
		return NULL;
	}
	Stack* temp = s->next;
	s->next = s->next->next;
	Node* tempNode = temp->node;
	free(temp);
	return tempNode;
}

// Return true if empty, false if not
int empty(Stack *s) {
	if(s->next == NULL) {
		return 1;
	} else {
		return 0;
	}
}

// Free's all contents of stack except for nodes
void destroy(Stack *s) {
	while(s != NULL) {
		Stack* temp = s;
		s = s->next;
		free(temp);
	}
}

// Converts string input into a package data type containing all package information
Package parseStringToPackage(char* str) {
	Package newPackage;
	newPackage.id = 'X';		// 'X' = (Unused), 'H' = Horizontal split, 'V' = Vertical split, 'N' = Numerical label node 
	newPackage.label = '0';
	newPackage.width = 0;
	newPackage.height = 0;
	newPackage.xcoord = 0;
	newPackage.ycoord = 0;
	int widthIdx = 0;
	int label, mult, heightIdx;
	int i = 0;
	double width, height, decimalMult;
	if(strcmp(str, "H") == 0) {
		newPackage.id = 'H';
	} else
	if(strcmp(str, "V") == 0) {
		newPackage.id = 'V';
	} else {
		// Set id to N
		newPackage.id = 'N';
		// Set label
		while(str[widthIdx++] != '(');
		i = widthIdx - 2;
		label = 0;
		mult = 1;
		while(i >= 0) {
			label += (int)(str[i--] - '0') * mult;
			mult *= 10;
		}
		newPackage.label = label;

		// Set width
		i = 11; // 11 characters to the right of widthIdx is end of width
		width = 0;
		mult = 1;
		decimalMult = 0.000001;
		while(i >= 0) {
			if(i == 11) {
				mult = (int)(str[widthIdx + i--] - '0');
				mult += (int)(str[widthIdx + i--] - '0') * 10;
				i -= 2;
			} else if(i != 1){
				width += ((double)(str[widthIdx + i--] - '0')) * decimalMult;
				decimalMult *= 10;
			} else {
				i--;
			}
		}
		i = 1;
		while(mult-- > 0) {
			i *= 10;
		}
		width *= i;
		newPackage.width = width;

		// Set height
		heightIdx = widthIdx;
		while(str[heightIdx++] != ',');
		i = 11; // 11 characters to the right of widthIdx is end of width
		height = 0;
		mult = 1;
		decimalMult = 0.000001;
		while(i >= 0) {
			if(i == 11) {
				mult = (int)(str[heightIdx + i--] - '0');
				mult += (int)(str[heightIdx + i--] - '0') * 10;
				i -= 2;
			} else if(i != 1){
				height += ((double)(str[heightIdx + i--] - '0')) * decimalMult;
				decimalMult *= 10;
			} else {
				i--;
			}
		}
		i = 1;
		while(mult-- > 0) {
			i *= 10;
		}
		height *= i;
		newPackage.height = height;
	}
	return newPackage;
}

void printPostOrder(Node* n, FILE *f) {
	if(n == NULL)
		return;
	printPostOrder(n->left, f);
	printPostOrder(n->right, f);
	if(n->value.id == 'N')
		fprintf(f, "%d %le %le %le %le\n", n->value.label, n->value.width, n->value.height, n->value.xcoord, n->value.ycoord);
}

Node *Load_From_File(char *Filename) {
	FILE *f;
	// Expected input can not have more than 38 chars, where %d(%le,%le)\n contains
	// INT_MAX, %le (positive), %le (positive): (10 + 1 + 12 + 1 + 12 + 2) = 38
	char buff[INPUT_MAX_LENGTH];
	if(!(f = fopen(Filename, "r"))) {
		fprintf(stderr, "File could not be loaded\n");
		return NULL;
	}

	Package tempPackage;
	Stack* auxiliaryStack = makeStack();
	Node *auxiliaryNode, *parent, *leftChild = NULL, *rightChild = NULL;
	while(fscanf(f, "%s", buff) != EOF) {
		tempPackage = parseStringToPackage(buff);
		if(tempPackage.id == 'N') {
			// Add to stack
			auxiliaryNode = malloc(sizeof(Node));
			auxiliaryNode->value = tempPackage;
			auxiliaryNode->left = NULL;
			auxiliaryNode->right = NULL;
			push(auxiliaryStack, auxiliaryNode);
		} else {
			// Create tree cluster
			auxiliaryNode = malloc(sizeof(Node));
			auxiliaryNode->value = tempPackage;
			parent = auxiliaryNode;
			if(!empty(auxiliaryStack)) {
				rightChild = pop(auxiliaryStack);
			}
			if(!empty(auxiliaryStack)) {
				leftChild = pop(auxiliaryStack);
			}
			
			parent->left = leftChild;
			parent->right = rightChild;

			// Assign 'V' and 'H' nodes their sizes (the package size of combine numerical nodes)
			if(parent->value.id == 'V') {
				if(leftChild->value.height > rightChild->value.height) {
					parent->value.height = leftChild->value.height;
				} else {
					parent->value.height = rightChild->value.height;
				}
				parent->value.width = leftChild->value.width + rightChild->value.width;
			} else {
				if(leftChild->value.width > rightChild->value.width) {
					parent->value.width = leftChild->value.width;
				} else {
					parent->value.width = rightChild->value.width;
				}
				parent->value.height = leftChild->value.height + rightChild->value.height;
			}
			parent->value.label = -1;

			push(auxiliaryStack, parent);
		}
	}
	auxiliaryNode = top(auxiliaryStack);
	destroy(auxiliaryStack);
	fclose(f);
	return auxiliaryNode;
}

void freeTree(Node *root) {
	if(root == NULL)
		return;
	freeTree(root->left);
	freeTree(root->right);
	free(root);
}

int Save_To_File(char *Filename, Node *root) {
	FILE *f;
	if(!(f = fopen(Filename, "w"))) {
		fprintf(stderr, "File could not be loaded\n");
		return -1;
	}

	printPostOrder(root, f);	

	freeTree(root);
	fclose(f);
	return 0; 
} 

void Perform_Packing(Node *root, int *max_label, double* x, double* y) {
	if(root == NULL) {
		return;
	}
	// If 'V' x coordinate of right is shifted, left remains
	// If 'H' y coordinate of left is shifted, right remains
	if(root->value.id == 'V') {
		root->left->value.xcoord = root->value.xcoord;
		root->left->value.ycoord = root->value.ycoord;
		root->right->value.xcoord = root->value.xcoord + root->left->value.width;
		root->right->value.ycoord = root->value.ycoord;
	} else if(root->value.id == 'H') {
		root->right->value.xcoord = root->value.xcoord;
		root->right->value.ycoord = root->value.ycoord;
		root->left->value.xcoord = root->value.xcoord;
		root->left->value.ycoord = root->value.ycoord + root->right->value.height;
	}
	// Update max_label (for screen dump)
	if(root->value.label > *max_label) {
		*max_label = root->value.label;
		*x = root->value.xcoord;
		*y = root->value.ycoord;
	}
	
	Perform_Packing(root->left, max_label, x, y);
	Perform_Packing(root->right, max_label, x, y);
}

