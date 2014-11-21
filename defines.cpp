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
    string C_INPUT  = "C_INPUT";
    string C_OUTPUT = "C_OUTPUT";
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
