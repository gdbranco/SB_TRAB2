#include "defines.h"
namespace run_type
{
    string PRE_PROCESS_EQU = "-p";
    string PRE_PROCESS_MACRO = "-m";
    string COMPILE = "-o";
}
namespace instructions
{
    string ADD    = "ADD";
    string SUB    = "SUB";
    string MULT   = "MULT";
    string DIV    = "DIV";
    string JMP    = "JMP";
    string JMPN   = "JMPN";
    string JMPP   = "JMPP";
    string JMPZ   = "JMPZ";
    string COPY   = "COPY";
    string LOAD   = "LOAD";
    string STORE  = "STORE";
    string INPUT  = "INPUT";
    string OUTPUT = "OUTPUT";
    string C_INPUT  = "C_INPUT";
    string C_OUTPUT = "C_OUTPUT";
	string S_INPUT = "S_INPUT";
	string S_OUTPUT = "S_OUTPUT";
    string STOP   = "STOP";
}
namespace diretivas
{
    string TEXT    = "TEXT";
    string DATA    = "DATA";
    string SECTION = "SECTION";
    string SPACE   = "SPACE";
    string CONST   = "CONST";
    string EQU     = "EQU";
    string IF      = "IF";
    string MACRO   = "MACRO";
    string END     = "END";
}
namespace nasm_functions
{
	string __print_string = "__print_string:\nenter 0,0\npush eax\npush ebx\npush ecx\npush edx\npush esi\nxor esi,esi\n__print_char:\nmov eax, 4\nmov ebx, 1\nmov ecx, [ebp+8]\nadd ecx, esi\nmov edx, 1\nint 0x80\n\ninc esi\n\nmov eax, [ebp+8]\ncmp byte [eax + esi -1], 0x0a\nje __end_print\n\njmp __print_char\n\n__end_print:\npop esi\npop edx\npop ecx\npop ebx\npop eax\nleave\nret 4\n";
	string __read_string = "__read_string:\nenter 0,0\npush ebx\npush ecx\npush edx\npush esi\nxor esi, esi\n__read_char:\nmov eax, 3\nmov ebx, 0\nmov ecx, [ebp + 8]\nadd ecx, esi\nmov edx, 1\nint 0x80\n\ninc esi\n\nmov eax, [ebp + 8]\ncmp byte [eax + esi - 1], 0x0A\nje __end_read\ncmp esi, [ebp + 12]\nje __end_read_full\njmp __read_char\n\n__end_read:\nmov ecx, [ebp + 8]\nadd ecx, esi\n;add ecx, 1\ndec si\nmov ax, si\nmov byte [ecx], 0x0A\njmp __end_read_string\n\n__end_read_full:\nmov ecx, [ebp + 8]\nadd ecx, esi\nmov byte [ecx - 1], 0x0a\nmov eax, 4\nmov ebx, 1\nmov ecx, __ERROR_MSG\nmov edx, __ERROR_MSG_SIZE\nint 0x80\nmov ax, 0\n__end_read_string:\npop esi\npop edx\npop ecx\npop ebx\nleave\nret 8\n";
}
