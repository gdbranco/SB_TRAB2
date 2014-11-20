#include <iostream>
#include <map>
#include "parser.h"
#include "defines.h"
using namespace std;

int main(int argc, char *argv[]) 
{
	PARSER::inicializa_paradas();
	inst_t *minst;
	minst = new inst_t("ADD");
	minst->arg_list.push_back("LABEL");

	cout << PARSER::translate(minst);
	delete minst;
	return 0;
}
