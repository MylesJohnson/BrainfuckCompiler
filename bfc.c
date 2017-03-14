#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bfc.h"

operationNode * readFile(const char* filename)
{
	FILE * inputFP = fopen(filename, "r");
	char * input;
	long i = -1;
	operationNode * head;

	if (inputFP) 
	{
		fseek(inputFP, 0, SEEK_END);
		long length = ftell(inputFP);
		fseek(inputFP, 0, SEEK_SET);
		input = malloc(length);
		if (input)
			fread(input, 1, length, inputFP);

		fclose(inputFP);
	} else {
		perror("Failed to open input file");
		exit(1);
	}

	operationNode * cur = NULL;
	while (input[++i]) {
		if(!strchr("+-<>.,[]", input[i]))
			continue;

		operationNode * temp = NULL;
		if(cur == NULL)
		{
			cur = head = malloc(sizeof(operationNode));
			head = cur;
		} else {
			cur->next = malloc(sizeof(operationNode));
			temp = cur;
			cur = cur->next;
		}
		
		cur->next = NULL;
		cur->prev = temp;

		switch (input[i])
		{
			case '+':
				cur->operation = INCREMENT;
				cur->value = 1;
				break;
			case '-':
				cur->operation = INCREMENT;
				cur->value = -1;
				break;
			case '>':
				cur->operation = POINTER_INCREMENT;
				cur->value = 1;
				break;
			case '<':
				cur->operation = POINTER_INCREMENT;
				cur->value = -1;
				break;
			case '.':
				cur->operation = WRITE;
				cur->value = -1;
				break;
			case ',':
				cur->operation = READ;
				cur->value = -1;
				break;
			case '[':
				cur->operation = OPEN;
				cur->value = -1;
				break;
			case ']':
				cur->operation = CLOSE;
				cur->value = -1;
				break;
		}
	}

	free(input);
	return head;
}

operationNode * preprocess(operationNode * input)
{
	operationNode * head = input;
	operationNode * cur = input;
	while(cur->next)
	{
		if(cur->value == 0)
		{
			//If it isn't going to do anything, why does it exist.
			if (cur == head)
				head = head->next;
			else
			{
				if(cur->next)
					cur->next->prev = cur->prev;
				if(cur->prev)
					cur->prev->next = cur->next;
			}

			operationNode * temp = cur;
			cur = cur->prev;
			free(temp);
			continue;
		}
		if(cur->operation == cur->next->operation && cur->operation == READ)
		{
			//No point in doing multiple reads on the same pointer location.
			operationNode * temp = cur->next;
			cur->next=cur->next->next;
			cur->next->prev = cur;
			free(temp);
			continue; // Don't move onto the next yet, we aren't ready.
		}
		if(cur->operation == INCREMENT && cur->next->operation == READ)
		{
			//No point in incrementing if we're just going to read after.
			if (cur == head)
				head = head->next;
			else
			{
				if(cur->next)
					cur->next->prev = cur->prev;
				if(cur->prev)
					cur->prev->next = cur->next;
			}

			operationNode * temp = cur;
			cur = cur->next;
			free(temp);
			continue;
		}
		if(cur->operation == cur->next->operation && cur->operation < READ)
		{
			//This node and the next node can be combind
			cur->value += cur->next->value;
			operationNode * temp = cur->next;
			cur->next=cur->next->next;
			cur->next->prev = cur;
			free(temp);
			continue; // Don't move onto the next yet, we aren't ready.
		}

		cur = cur->next;
	}
	return head;
}

void writeFile(const char * filename, operationNode * assembly)
{
	FILE *outputFP = fopen(filename, "w");
	if (outputFP == NULL)
	{
		printf("Failed to open output file");
		exit(1);
	}

	fputs(ASM_HEADER, outputFP);

	uint * stack = calloc(255, sizeof(char));
	uint top = 0;
	uint loop = 0;
	operationNode * cur = assembly;
	while(cur)
	{
		switch (cur->operation)
		{
			case INCREMENT:
				fprintf(outputFP, ASM_INCREMENT, cur->value);
				break;
			case POINTER_INCREMENT:
				fprintf(outputFP, ASM_POINTER_INCREMENT, cur->value);
				break;
			case WRITE:
				fputs(ASM_WRITE, outputFP);
				break;
			case READ:
				fputs(ASM_READ, outputFP);
				break;
			case OPEN: ; //Because C is stupid and you can't declare vars after a label...
				stack[top++] = ++loop;
				fprintf(outputFP, ASM_OPEN, loop, loop);
				break;
			case CLOSE: ; //Because C is stupid and you can't declare vars after a label...
				fprintf(outputFP, ASM_CLOSE, stack[--top], stack[top - 1]);
				break;
		}
		cur = cur->next;
	}

	fputs(ASM_FOOTER, outputFP);
	fclose(outputFP);
	free(stack);
}

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Invalid number of args\n");
		exit(1);
	}

	operationNode * input = readFile(argv[1]);
	operationNode * output;
	if (input)
		//output = input;
		output = preprocess(input);
	else {
		perror("File was empty");
		exit(1);
	}
	
	if (output)
		writeFile(argv[2], output);
	else
		fprintf(stderr, "No output generated\n");
}