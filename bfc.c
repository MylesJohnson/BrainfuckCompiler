#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <argp.h>

#include "bfc.h"
#include "linkedlist.c"

operationNode * readFile(const char* filename)
{
	FILE * inputFP = fopen(filename, "r");
	char * input;
	long i = -1;
	operationNode * head = NULL;

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

	operationNode * prev = NULL;
	while (input[++i]) {
		if(!strchr("+-<>.,[]", input[i]))
			continue;
		
		operationNode * cur = malloc(sizeof(operationNode));

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

		if(head == NULL)
			head = cur;
		list_append(prev, cur);
		prev = cur;
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
			operationNode * temp = cur->next;
			list_remove(cur);
			cur = temp;
			continue;
		}
		if(cur->operation == cur->next->operation && cur->operation == READ)
		{
			//No point in doing multiple reads on the same pointer location.
			list_remove(cur->next);
			continue; // Don't move onto the next yet, we aren't ready.
		}
		if(cur->operation == INCREMENT && cur->next->operation == READ)
		{
			operationNode * temp = cur->next;
			list_remove(cur);
			cur = temp;
			continue;
		}
		if(cur->operation == cur->next->operation && cur->operation < READ)
		{
			//This node and the next node can be combind
			cur->value += cur->next->value;
			list_remove(cur->next);
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
		perror("Failed to open output file");
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
				top--;
				fprintf(outputFP, ASM_CLOSE, stack[top], stack[top]);
				break;
		}
		operationNode * tmp = cur;
		cur = cur->next;
		free(tmp);
	}

	fputs(ASM_FOOTER, outputFP);
	fclose(outputFP);
	free(stack);
}

char* remove_extension(char* basename) {
	char* output;

	if (basename == NULL)
		return NULL;

	char* lastdot = strrchr(basename, '.');

	if(lastdot == NULL)
		lastdot = strrchr(basename, '\0');

	// The +2 is to allow the '.s' be added later
	if ((output = malloc(lastdot - basename + 3)) == NULL)
		return NULL;

	strncpy(output, basename, lastdot - basename);
	return output;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	arguments *args = state->input;

	switch (key)
	{
		case 'o':
			args->output_file = arg;
			break;

		case 'O':
			args->optimize = 0;
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 1)
				argp_usage (state);
			args->input_file = arg;
			break;

		case ARGP_KEY_END:
			if (state->arg_num < 1)
				argp_usage (state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv)
{
	arguments args;
	args.output_file = NULL;
	args.optimize = 1;

	argp_parse(&argp, argc, argv, 0, 0, &args);
	if(!args.output_file)
	{
		args.output_file = remove_extension(basename(args.input_file));
		strncat(args.output_file, ".s", 2);
	}

	operationNode * input = readFile(args.input_file);
	operationNode * output;
	if (input) {
		if(args.optimize)
			output = preprocess(input);
		else
			output = input;
	} else {
		perror("File was empty");
		exit(1);
	}
	
	if (output)
		writeFile(args.output_file, output);
	else
		fprintf(stderr, "No output generated\n");
}
