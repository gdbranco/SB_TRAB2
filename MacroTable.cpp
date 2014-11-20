#include "MacroTable.h"
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>


/* Cria a macro em uma linha referenciada pelo iterator code_it 
 * e guarda na tabela interna do objeto.
 * Retorna o iterador para o final da macro. */
code_t::iterator MacroTable::create_macro(code_t::iterator &it, code_t &code) {
	std::string name;
	std::vector<Linha> macro_body;
	bool success = false;
	std::vector<Linha> macro_result;
	Macro new_macro;

	/*Erro se o segundo token não é MACRO*/
	if(it->tokens[1] != "MACRO") {
		return code.end();

	/*Erro se o primeiro token não é uma label válida*/
	} else if(!PARSER::islabel(it->tokens[0])) {
		return code.end();

	} else {
		name = it->tokens[0];
		name.erase(name.end() - 1);

		it++;

	/*	Vai adicionando as linhas na variável de corpo da macro até encontrar "endmacro".
		Incrementa o iterador ao fazer isso.
		Se chegar ao final sem encontrar o "endmacro" (success = false), um erro ocorre. */
		while(it != code.end()) {
			std::vector<string>::iterator aux;
			aux = std::find(it->tokens.begin(), it->tokens.end(), "END");
			if( aux == it->tokens.end() ) {
				macro_body.push_back(*it);
				it++;

			} else {
				success = true;
				break;
			}
		}

		if (not success) {
			return code.end();
		}

		new_macro = Macro(name, macro_body);
	 	this->macros.push_back(new_macro);
		
	}
	return it;
}

 /* Função que retorna o corpo de uma macro.
  * Recebe o iterador referente à linha da chamada da macro. */
code_t MacroTable::get_macro(code_t::iterator it) {
	std::string macro_name;
	std::vector<Macro>::iterator macro_it;
	std::vector<Linha> macro_body;

	/*Pega o nome da macro e seus argumentos. Resolve caso tenha label.*/
	if (PARSER::islabel(it->tokens[0])) {
		macro_name = it->tokens[1];
	} else {
		macro_name = it->tokens[0];
	}
	
	/*Procura pela macro*/
	macro_it = this->macros.begin();
	while(macro_it != this->macros.end()) {
		if (((*macro_it).label == macro_name)) {
			macro_body = (*macro_it).text_code;
			break;
		}
		macro_it++;
	}

	return macro_body;

}

/*Confere se possui a macro*/
bool MacroTable::has(std::string name) {
	std::vector<Macro>::iterator it;
	it = this->macros.begin();

	while(it != this->macros.end()) {
		if(it->label == name) {
			return true;
		}
		it++;
	}

	return false;
}
