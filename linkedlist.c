#include <stdlib.h>

#include "linkedlist.h"

void list_append(operationNode* node, operationNode* new)
{
	operationNode* tmp = NULL;
	if(node)
	{
		tmp = node->next;
		node->next = new;
	}

	new->next = tmp;
	new->prev = node;

	if(tmp)
		tmp->prev = new;
}

void list_remove(operationNode* node)
{
	if(node->next)
		node->next->prev = node->prev;
	if(node->prev)
		node->prev->next = node->next;
	free(node);
}
