#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H 

#include <vector>
#include <string>
#include "defines.h"
#include "parser.h"

class MacroTable {
private:
	std::vector<Macro> macros;

public:
	MacroTable() {}
	code_t::iterator create_macro(code_t::iterator &code_it, code_t &code);
	std::vector<Linha> get_macro(code_t::iterator it);
	bool has(std::string name);

};

#endif //MACRO_TABLE_H
