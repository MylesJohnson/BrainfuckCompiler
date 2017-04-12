#ifndef bfc_header
#define bfc_header

typedef struct {
	char *input_file;
	char *output_file;
	char optimize;
} arguments;

static char doc[] = "BFC - A brainfuck compiler written in C. Goes directly to ASM";
static char args_doc[] = "infile";
static struct argp_option options[] = {
	{"output", 'o', "FILE", 0, "Output to FILE instead of default(infile.s)" },
	{"disableOptimization", 'O', 0, 0, "Disable optimization"},
	{ 0 }
};


typedef enum {
	INCREMENT,
	POINTER_INCREMENT,
	READ,
	WRITE,
	OPEN,
	CLOSE,
	OTHER
} op_t;

typedef struct node {
	op_t operation;
	int value;
	struct node * next;
	struct node * prev;
} operationNode;


const char * ASM_INCREMENT = "\taddb $%i, (%%edi)\n";
const char * ASM_POINTER_INCREMENT = "\tadd $%i, %%edi\n";
const char * ASM_WRITE = "\tmovl $4, %eax\n"
						 "\tmovl $1, %ebx\n"
						 "\tmovl %edi, %ecx\n"
						 "\tmovl $1, %edx\n"
						"\tint $0x80\n";
const char * ASM_READ = "\tmovl $3, %eax\n"
						 "\tmovl $0, %ebx\n"
						 "\tmovl %edi, %ecx\n"
						 "\tmovl $1, %edx\n"
						 "\tint $0x80\n";
const char * ASM_OPEN =  "\tcmpb $0, (%%edi)\n"
						 "\tjz .LE%u\n"
						 ".LB%u:\n";
const char * ASM_CLOSE =  "\tcmpb $0, (%%edi)\n"
						  "\tjnz .LB%u\n"
						  ".LE%u:\n";


const char * ASM_HEADER = "\t.comm tape,30000\n"
						  "\n"
						  "\t.globl main\n"
						  "\t.type main, @function\n"
						  "main:\n"
						  "\tmovl $tape, %edi\n";

const char * ASM_FOOTER = "\tmovl $0, %eax\n"
						  "\tret\n";

#endif