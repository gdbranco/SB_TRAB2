#ifndef DEFINE_H
#define DEFINE_H
#include <utility>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

typedef struct _mem_acc
{
	unsigned int PC;
	bool is_jump;
	unsigned int linha;
	_mem_acc(){}
	_mem_acc(const unsigned int _PC, const bool _is_jump, const unsigned int _linha):
	PC(_PC),is_jump(_is_jump),linha(_linha){}
}mem_acc_t;

typedef struct _smb
{
    string simb;
    int value;
    bool def;
    bool is_const;
    vector<int> lista_end;
	vector<bool> lista_mem_changer;
	vector<bool> div_list;
	vector<int> lista_nlinha;
    _smb() {}
    _smb(const string _simb, const int _value,const bool _def,const vector<int> _lista_end,const vector<bool> _lista_mem_changer,const vector<bool> _div_list,const vector<int> _lista_nlinha):
        simb(_simb),value(_value),def(_def),is_const(false),lista_end(_lista_end),lista_mem_changer(_lista_mem_changer),div_list(_div_list),lista_nlinha(_lista_nlinha) {}
    _smb(const string _simb, const int _value,const bool _def):
        simb(_simb),value(_value),def(_def), is_const(false) {}
    friend ostream& operator<<(ostream& os,const _smb& it)
    {
        os << it.simb << ' ';
        os << it.value << ' ';
        os << it.def   << ' ';
        os << it.is_const  << ' ';
        for(unsigned int i=0; i<it.lista_end.size(); i++)
        {
            os << it.lista_end[i];
            if(i!=it.lista_end.size()-1)
            {
                os << "->";
            }
        }
        return os;
    }
} smb_t;
typedef vector<smb_t> tsmb_t;

struct space_t 
{
	string label;
	string num;
	space_t(string _label, string _num) : label(_label), num(_num) {}
};

typedef struct sum_t
{
    string simb;
    int signal;
    int position;
    sum_t() {}
    sum_t(const string _simb, int _signal, const int _position):
        simb(_simb),signal(_signal),position(_position) {}
    friend ostream& operator<<(ostream& os,const sum_t& it)
    {
        os << it.simb << ' ';
        os << it.position << ' ';
        return os;
    }
} sum_t;
typedef vector<sum_t> tsum_t;

typedef struct _inst
{
	string name;
	vector<string> arg_list;
    _inst() {}
    _inst(const string _inst_name):
        name(_inst_name){}
} inst_t;
typedef vector<inst_t> tinst_t;

typedef struct _linha
{
    int nlinha;
    vector<string> tokens;
    _linha() {}
    _linha(int _nlinha, vector<string> _tokens):
        nlinha(_nlinha),tokens(_tokens) {}
    friend ostream& operator<<(ostream& os, const _linha& it)
    {
        for(unsigned int i =0; i<it.tokens.size(); i++)
        {
            os << it.tokens[i];
            if(i!=it.tokens.size()-1)
            {
                os << ' ';
            }
        }
        return os;
    }
} linha_t, Linha;
typedef vector<linha_t> code_t;

/*Classes para abstração da memória de códigos para a main*/
class virtual_code_wrapper
{ };

class	text_code_wrapper : public virtual_code_wrapper
{
	public:
		code_t	code_impl;
		text_code_wrapper(code_t _code) : code_impl(_code) {}
};

class	obj_code_wrapper : public virtual_code_wrapper
{
	public:
		vector<int>	obj_code;
		obj_code_wrapper(vector<int> _code) : obj_code(_code) {}
};
// ----------

struct Macro
{
    string label;
    code_t text_code;
    Macro() {}
    Macro(string _label, code_t _text_code) : label(_label), text_code(_text_code) { }
};

typedef struct _define
{
    string label;
    int value;
    _define() {}
    _define(string _label,int _value):
        label(_label),value(_value) {}
    friend bool operator==(_define& define1, _define& define2)
    {
        return (define1.label == define2.label);
    }
    friend ostream& operator<<(ostream& os, const _define& it)
    {
        os << it.label << ' ' << it.value;
        return os;
    }
} define_t, Define;


namespace run_type
{
extern string PRE_PROCESS_EQU;
extern string PRE_PROCESS_MACRO;
extern string COMPILE;
}

namespace sections
{
extern string S;
extern string TEXT;
extern string DATA;
}

namespace instructions
{
extern string ADD;
extern string SUB;
extern string MULT;
extern string DIV;
extern string JMP;
extern string JMPN;
extern string JMPP;
extern string JMPZ;
extern string COPY;
extern string LOAD;
extern string STORE;
extern string C_INPUT;
extern string C_OUTPUT;
extern string STOP;
}

namespace diretivas
{
extern string TEXT;
extern string DATA;
extern string SECTION;
extern string SPACE;
extern string CONST;
extern string EQU;
extern string IF;
extern string MACRO;
extern string END;
}
#endif // DEFINE_H
