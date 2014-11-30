#include <iostream>
#include <map>
#include "parser.h"
#include "defines.h"
using namespace std;

int main(int argc, char *argv[]) 
{
	if(argc<2)
	{
		cout << "Uso : ./tradutor.out file_entrada";
		return 1;
	}
	string arq_nome(argv[1]);
	size_t found = arq_nome.find_last_of(".");
	if(found==string::npos)
	{
		arq_nome += ".asm";
	}
	code_t my_mem = PARSER::toMEM(arq_nome.c_str()); 
	my_mem = PARSER::run_preproc(my_mem);
	my_mem = PARSER::run_macros(my_mem);
	fstream arq;
	found = arq_nome.find_last_of(".");
	if(found)
	{
		arq_nome = arq_nome.substr(0,found);
	}
	arq_nome += ".s";
	arq.open(arq_nome.c_str(),std::fstream::out | std::fstream::trunc);
	arq << PARSER::run_montador(my_mem);

	return 0;
}
