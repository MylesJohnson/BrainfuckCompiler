#ifndef list_header
#define list_header

typedef struct node {
	op_t operation;
	int value;
	struct node * next;
	struct node * prev;
} operationNode;

extern void list_append(operationNode* list, operationNode* new);
extern void list_remove(operationNode* list);

#endif