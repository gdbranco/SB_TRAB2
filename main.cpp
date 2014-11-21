#include <iostream>
#include <map>
#include "parser.h"
#include "defines.h"
using namespace std;

int main(int argc, char *argv[]) 
{

	code_t my_mem = PARSER::toMEM("teste.asm"); 
	PARSER::run_montador(my_mem);

	return 0;
}
