#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "bfc.h"

op_t strToOP(char op)
{
	switch (op)
	{
		case '+':
			return PLUS;
		case '-':
			return MINUS;
		case '>':
			return GTR;
		case '<':
			return LESS;
		case '.':
			return DOT;
		case ',':
			return COMMA;
		case '[':
			return OPEN;
		case ']':
			return CLOSE;
		default:
			return OTHER;
	}
}

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

	head = malloc(sizeof(operationNode));
	operationNode * cur = head;
	while (input[++i]) {
		if(strToOP(input[i]) == OTHER)
			continue;

		cur->next = malloc(sizeof(operationNode));
		operationNode * temp = cur;

		cur = cur->next;
		cur->next = NULL;
		cur->prev = temp;
		cur->operation = strToOP(input[i]);
		cur->value = 1;
	}

	free(input);
	return head->next;
}

operationNode * preprocess(operationNode * input)
{
	operationNode * cur = input;
	while(cur->next)
	{
		if(cur->operation == cur->next->operation && cur->operation < DOT)
		{
			//This node and the next node can be combind
			cur->value++;
			operationNode * temp = cur->next;
			cur->next=cur->next->next;
			free(temp);
			continue; // Don't move onto the next yet, we aren't ready.
		}
		cur = cur->next;
	}
	return input;
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
	uint top, loop = 0;
	operationNode * cur = assembly;
	while(cur)
	{
		switch (cur->operation)
		{
			case PLUS:
				fprintf(outputFP, ASM_PLUS, cur->value);
				break;
			case MINUS:
				fprintf(outputFP, ASM_MINUS, cur->value);
				break;
			case GTR:
				fprintf(outputFP, ASM_GTR, cur->value);
				break;
			case LESS:
				fprintf(outputFP, ASM_LESS, cur->value);
				break;
			case DOT:
				fputs(ASM_DOT, outputFP);
				break;
			case COMMA:
				fputs(ASM_COMMA, outputFP);
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