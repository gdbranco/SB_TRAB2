#include <iostream>
#include <map>
#include "parser.h"
#include "defines.h"
using namespace std;

int main(int argc, char *argv[]) 
{
	
	code_t my_mem = PARSER::toMEM(argv[1]); 
	my_mem = PARSER::run_preproc(my_mem);
	my_mem = PARSER::run_macros(my_mem);
	PARSER::run_montador(my_mem);

	return 0;
}
