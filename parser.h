#ifndef PARSER_H
#define PARSER_H
#define OK 1
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <utility>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "defines.h"
#include "MacroTable.h"
//TODO:
using namespace std;
class PARSER
{
private:
    vector<define_t> defines_list;
    tsmb_t simb_list;
    code_t make_listaEQU(code_t _code);
    code_t preproc(code_t _code);
    code_t passagem_macros(code_t _code);
    vector<int> passagem_unica(code_t code);
    int define_exists(const define_t procura);
    int symbol_exists(string procura);
	static string ReplaceAll(std::string str, const std::string& from, const std::string& to);
public:
	static string translate(inst_t* rinst);
    static vector<string> instruction_list;
	static std::map<string, string> translation;
    /**Metodos**/
    static int islabel(const string _label);
    static int isdir(const string _dir);
    static bool isinst(const string _inst,inst_t* inst);
    static int iscomment(const string _comment);
    static bool isSymbol(string simb);
    static bool isNumber(string simb);
    static code_t run_preproc(code_t _code);
    static code_t run_macros(code_t code);
    static vector<int> run_montador(code_t code);
    static code_t toMEM(const string nome_arq);
    static string retiraComentarios(string _linha);
    static void inicializa_paradas();
};
#endif // PARSER_H
