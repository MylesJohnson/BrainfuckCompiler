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
	// Currently does no preprocessing 
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
				fputs(ASM_PLUS, outputFP);
				break;
			case MINUS:
				fputs(ASM_MINUS, outputFP);
				break;
			case GTR:
				fputs(ASM_GTR, outputFP);
				break;
			case LESS:
				fputs(ASM_LESS, outputFP);
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