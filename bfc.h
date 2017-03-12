#ifndef bfc_header
#define bfc_header

const char * ASM_PLUS = "\taddb $1, (%edi)\n";
const char * ASM_MINUS = "\tsubb $1, (%edi)\n";
const char * ASM_GTR = "\tadd $1, %edi\n";
const char * ASM_LESS = "\tsub $1, %edi\n";
const char * ASM_DOT = "\tmovl $4, %eax\n"
            		   "\tmovl $1, %ebx\n"
            		   "\tmovl %edi, %ecx\n"
            		   "\tmovl $1, %edx\n"
            		   "\tint $0x80\n";
const char * ASM_COMMA = "\tmovl $3, %eax\n"
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