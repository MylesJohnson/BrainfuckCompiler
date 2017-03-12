#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bfc.h"

char * readFile(const char* filename)
{
	FILE * inputFP = fopen(filename, "r");
	char * input;

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

	return input;
}

char * preprocess(char * inputCode)
{
	// Currently does no preprocessing 
	return inputCode;
}

char * compile(char * inputCode)
{
	long i = 0;
	uint * stack = calloc(255, sizeof(char));
	uint top, loop = 0;
	char * output = calloc(9999999, sizeof(char));

	while (inputCode[i]) {
		switch (inputCode[i])
		{
			case '+':
				strcat(output, ASM_PLUS);
				break;
			case '-':
				strcat(output, ASM_MINUS);
				break;
			case '>':
				strcat(output, ASM_GTR);
				break;
			case '<':
				strcat(output, ASM_LESS);
				break;
			case '.':
				strcat(output, ASM_DOT);
				break;
			case ',':
				strcat(output, ASM_COMMA);
				break;
			case '[': ; //Because C is stupid and you can't declare vars after a label...
				char * tmp = calloc(512, sizeof(char));
				stack[top++] = ++loop;
				sprintf(tmp, ASM_OPEN, loop, loop);
				strcat(output, tmp);
				free(tmp);
				break;
			case ']': ; //Because C is stupid and you can't declare vars after a label...
				char * tmp2 = calloc(512, sizeof(char));
				sprintf(tmp2, ASM_CLOSE, stack[--top], stack[top - 1]);
				strcat(output, tmp2);
				free(tmp2);
				break;
			default:
				break;
		}
		++i;
	}
	return output;
}

void writeFile(const char * filename, char * assembly)
{
	FILE *outputFP = fopen(filename, "w");
	if (outputFP == NULL)
	{
		printf("Failed to open output file");
		exit(1);
	}
	
	fprintf(outputFP, "%s%s%s", ASM_HEADER, assembly, ASM_FOOTER);
	fclose(outputFP);
}

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Invalid number of args\n");
		exit(1);
	}

	char * input = preprocess(readFile(argv[1]));
	char * output;

	if (input)
		output = compile(input);
	else {
		perror("File was empty");
		exit(1);
	}
	
	if (output != "")
		writeFile(argv[2], output);
	else
		fprintf(stderr, "No output generated\n");
}