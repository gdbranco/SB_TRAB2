#include "parser.h"
vector<string> PARSER::instruction_list;
map<string, string> PARSER::translation;
string PARSER::retiraComentarios(string _linha)
{
    size_t found = _linha.find(';');
    if(found!=string::npos)
    {
        //        cout << _linha << endl;
        //        cout << found;
        //        cin.get();
        if(found == 0)
        {
            _linha = "";
        }
        else
        {
            _linha = _linha.substr(0,found-1);
        }
    }
    return _linha;
}
void PARSER::inicializa_paradas()
{
    instruction_list.push_back(instructions::ADD);
    instruction_list.push_back(instructions::SUB);
    instruction_list.push_back(instructions::MULT);
    instruction_list.push_back(instructions::DIV);
    instruction_list.push_back(instructions::JMP);
    instruction_list.push_back(instructions::JMPN);
    instruction_list.push_back(instructions::JMPP);
    instruction_list.push_back(instructions::JMPZ);
    instruction_list.push_back(instructions::COPY);
    instruction_list.push_back(instructions::LOAD);
    instruction_list.push_back(instructions::STORE);
    instruction_list.push_back(instructions::INPUT);
    instruction_list.push_back(instructions::OUTPUT);
    instruction_list.push_back(instructions::STOP);

	translation["STOP"] = "mov eax,1\n\
							mov ebx,0\n\
							int 0x80";

	translation["C_INPUT"] = "mov eax,3\n\
								mov ebx,0\n\
								mov ecx, _L1\n\
								mov edx, 1\n\
								int 0x80";

	translation["C_OUTPUT"] = "mov eax,4\n\
								mov ebx,1\n\
								mov ecx, _L1\n\
								mov edx,1\n\
								int 0x80";

	translation["JMP"] = "jmp _L1";

	translation["JMPZ"] = "cmp ax,0\n\
							je _L1";

	translation["JMPP"] = "cmp ax,0\n\
							jg _L1";

	translation["JMPN"] = "cmp ax,0\n\
							jl _L1";

	translation["LOAD"] = "mov ax,_L1";

	translation["STORE"] = "mov [_L1],eax";

	translation["ADD"] = "add ax, _L1";

	translation["SUB"] = "sub ax,_L1";

	translation["MULT"] = "mul _L1";

	translation["DIV"] = "div _L1";

	translation["COPY"] = "copy _L1";
}

code_t PARSER::toMEM(string nome_arq)
{
    inicializa_paradas();
    code_t memoria;
    vector<string> _tokens;
    int _linha=0;
    fstream sc;
    char* pch;
    sc.open(nome_arq.c_str());
    string s;
    while(getline(sc,s))
    {
        _linha++;
        s = retiraComentarios(s);
        if(s!="")
        {
            pch = strtok((char*)s.c_str(),"\t ,");
            while(pch!=NULL)
            {
                transform(pch,pch+strlen(pch),pch,static_cast<int(*)(int)>(toupper));
                _tokens.push_back(string(pch));
                pch = strtok(NULL,"\t ,");
            }
            memoria.push_back(linha_t(_linha,_tokens));
        } 
		_tokens.clear();
    }
    sc.close();
    return memoria;
}
code_t PARSER::preproc(code_t _code)
{
    bool erase_currentline;
    bool erase_nextline;
    bool erro;
    bool achei;
    code_t code = _code;
    code_t::iterator linha = code.begin();
    vector<string>::iterator token;
    vector<define_t>::iterator define;
    if(!code.empty())
    {
        while(linha!=code.end())
        {
            erase_currentline = false;
            erase_nextline    = false;
            /**Checa se a linha eh um IF e se a proxima � valida**/
            if((find(linha->tokens.begin(),linha->tokens.end(),diretivas::IF)!=linha->tokens.end()))
            {
                token = linha->tokens.begin();
                /**Substitui os labels definidos na linha do IF por seu valor EQU**/
                while(token != linha->tokens.end())
                {
                    achei = false;
                    define = defines_list.begin();
                    while(define!=defines_list.end() && !achei)
                    {
                        erro=false;
                        stringstream ss;
                        ss << define->value;
                        if(*token == define->label.substr(0,define->label.length()-1))
                        {
                            *token = ss.str();
                            achei = true;
                        }
                        /**Se a linha nao for um EQU definido e nao for uma diretiva a definicao nao existe**/
                        else
                        {
                            if(!isdir(*token))
                            {
                                //                                cout << "token nao eh dir nem definido : " << *token;
                                //                                cin.get();
                                erro = true;
                            }
                        }
                        define++;
                    }
                    token++;
                }
                /**Se houver erro o coloca na lista**/
                token = linha->tokens.begin();
                /**Se nao houver erros faz a avaliacao do IF**/
                while(token != linha->tokens.end() && !erro)
                {
                    if(*token == diretivas::IF) //Essa condicao � inutil, retirar depois
                    {
                        erase_currentline = true;
                    }
                    else if(*token == "0")
                    {
                        erase_nextline = true;
                    }
                    token++;
                }
                if(erase_nextline)
                {
                    code.erase(linha+1);
                }
                if(erase_currentline)
                {
                    code.erase(linha);
                }
            }
            /**Tenta substituir caso um define seja achado fora de um IF**/
            else
            {
                define = defines_list.begin();
                while(define!=defines_list.end())
                {
                    if(find(linha->tokens.begin(),linha->tokens.end(),define->label.substr(0,define->label.length()-1))!=linha->tokens.end())
                    {
                        token = linha->tokens.begin();
                        while(token!=linha->tokens.end())
                        {
                            if(*token == define->label.substr(0,define->label.length()-1))
                            {
                                stringstream ss;
                                ss << define->value;
                                *token = ss.str();
                            }
                            token++;
                        }
                    }
                    define++;
                }
            }
            if(!erase_currentline)
                linha++;
        }
    }
    return code;
}
code_t PARSER::make_listaEQU(code_t _code)
{
    code_t code = _code;
    code_t::iterator linha = code.begin();
    vector<string>::iterator token;
    bool erased;
    bool labeled;
    bool EQU;
    int label_counter;
    int value;
    int erro;
    string label;
    if(!code.empty())
    {
        //Procura SECTION DATA
        while(linha!=code.end())
        {
            if((find(linha->tokens.begin(),linha->tokens.end(),diretivas::SECTION) != linha->tokens.end()) \
                    && \
                    (find(linha->tokens.begin(),linha->tokens.end(),diretivas::DATA) != linha->tokens.end()))
            {
                linha++;
                break; // Sai do loop com a linha no local correto SECTION DATA
            }
            linha++;
        }
        /**Procura as labels**/
        while(linha!=code.end())
        {
            erased = false;
            labeled = false;
            EQU = false;
            erro = 0;
            label.clear();
            label_counter=0;
            token = linha->tokens.begin();
            while(token!=linha->tokens.end())
            {
                if(islabel(*token))
                {
                    label = *token;
                    labeled = true;
                    label_counter++;
                }
                else if(token->find(diretivas::EQU)!=string::npos)
                {
                    EQU = true;
                }
                else
                {
                    value = atoi(token->c_str());
                }
                token++;
            }
            /**Apos passar pela linha pode-se tentar montar um define**/
            if(EQU)
            {
                if(labeled)
                {
                    if(label_counter<2)
                    {
                        define_t _aux(label,value);
                        if(!define_exists(_aux))
                        {
                            defines_list.push_back(_aux);
                            code.erase(linha);
                            erased = true;
                        }
					}
                }
            }

            if(!erased)
                linha++;
        }

    }
    return code;
}
code_t PARSER::run_preproc(code_t code)
{
    PARSER p;
    code = p.make_listaEQU(code);
    code = p.preproc(code);
    return code;
}

code_t PARSER::passagem_macros(code_t _code)
{
    code_t::iterator it;
    code_t::iterator m_beg, m_end;
    code_t macro_result;
    code_t code(_code);
    MacroTable mct;
	bool section = false;

    it = code.begin();
    while(it != code.end())
    {
        if (std::find(it->tokens.begin(), it->tokens.end(), "SECTION") != it->tokens.end()) {
			if (std::find(it->tokens.begin(), it->tokens.end(), "DATA") != it->tokens.end()) {
				section = false;

			} else if (std::find(it->tokens.begin(), it->tokens.end(), "TEXT") != it->tokens.end()) {
				section = true;

			} else {
				section = false;
			}
		}
		
        /*Se a linha possui "macro", � uma declara��o que deve ser colocada na tabela.*/
        if (std::find(it->tokens.begin(), it->tokens.end(), "MACRO") != it->tokens.end())
        {
			/*Guarda o indice do iterador para recuper�-lo ap�s apagar do vetor*/
			int aux = it - code.begin();
			m_beg = it;
			m_end = mct.create_macro(it, code);
			if(m_end != code.end())
			{
				code.erase(m_beg, m_end + 1);
			}
			it = code.begin() + aux -1;


            /*Caso contr�rio, confere se possui label e declara��o e substitui o c�digo.*/
        }
        else if ((PARSER::islabel(it->tokens[0]))   )
        {
			if(it->tokens.size() > 1) {
					if(mct.has(it->tokens[1])) {
						macro_result = mct.get_macro(it);
						code.erase(it);
						code.insert(it, macro_result.begin(), macro_result.end());
					}
				}

            /*Substitui a chamada pelo corpo da macro*/
        }
        else if (section && mct.has(it->tokens[0])  )
        {
			macro_result = mct.get_macro(it);
			code.erase(it);
			code.insert(it, macro_result.begin(), macro_result.end());

		}
        it++;
    }
    return code;
}

/**Metodo montador**/
vector<int> PARSER::passagem_unica(code_t code)
{
    unsigned int PC=0;
    int sinal;
    code_t _code = code;
    tsum_t sum_list;
    code_t::iterator linha = _code.begin();
    tsmb_t::iterator last_symbol;
    unsigned int increment_add;
    bool space_found, const_found, is_soma, has_label;
	bool section_text = true;
	bool is_inst = false;
	int end_first_section = 0;
	bool text_first=true;
	inst_t *rinst;

    while(linha!=_code.end())
    {
        space_found = false;
        const_found = false;
        is_soma = false;
		has_label = false;

        }
        sinal = 1;
        vector<string>::iterator token = linha->tokens.begin();


        while(token!=linha->tokens.end())
        {
            increment_add = 1; //Ao chegar num novo token o incremento do endereco eh sempre 1

			/*Confere se tem soma ou subtra��o*/
//TOKEN == +
            if ((*token) == "+")
            {
				/*Confere se a express�o acaba prematuramente*/
				is_soma = true;
				increment_add = 0;
				if(rinst->arg_list.size() > 0) {
					rinst->arg_list[rinst->arg_list.size() - 1] += " + ";
				}	
            }
//TOKEN == -
            else if ((*token) == "-")
            {
				is_soma = true;
				sinal = -1;
				increment_add = 0;
				if(rinst->arg_list.size() > 0) {
					rinst->arg_list[rinst->arg_list.size() - 1] += " - ";
				}	
            }
//TOKEN anterior � + ou -
            else if (is_soma)
            {
                increment_add = 0;
				/*Se � numero direto, soma*/
                if (isNumber(*token))
                {
					if(rinst->arg_list.size() > 0) {
						rinst->arg_list[rinst->arg_list.size() - 1] += (*token);
					}	
                }
            }
//TOKEN anterior � SPACE
            else if(space_found)
			{
				if(token!=linha->tokens.end())
				{
					if (isNumber(*token)) {
						increment_add = atoi(token->c_str());
						increment_add--;

					}
				}
			}
//TOKEN anterior � CONST
            else if(const_found)
            {
				/////
            }
//TOKEN � uma label
            else if(islabel(*token))
            {
					increment_add = 0; //Se for uma label n�o aumenta o endere�o
					int i = symbol_exists(token->substr(0, token->length() - 1));

					/*Se label ja existe*/
					if (i > -1)
					{
						simb_list[i].def = true;
						simb_list[i].value = PC;
						last_symbol = simb_list.begin() + i;

					}
					else
					{
						simb_list.push_back(smb_t(token->substr(0,token->length()-1), PC, true));
						last_symbol = simb_list.end() - 1;
					}
					has_label = true;
            }
//TOKEN � uma instru��o
            else if(isinst(*token,rinst)) /**Refazer para melhorar a estrutura de instrucoes e diretivas**/
            {
				is_inst = true;
            }
//TOKEN � uma diretiva
            else if(isdir(*token))
            {
                increment_add=0;
                if(*token == diretivas::SPACE)
                {
                    space_found = true;
                    increment_add=1;
                }
                else if(*token == diretivas::CONST)
                {
                    const_found = true;
                    if(simb_list.size() > 0)
                    {
                        last_symbol->is_const = true;
                    }
				}
				else if(*token == diretivas::SECTION)
				{
					if( *(token + 1) == "TEXT" ) {
						if(PC == 0)
						{
							text_first = true;
							section_text = true;	
						}
						if(!section_text)
						{
							end_first_section = PC;
						}
						section_text = true;
					} else if( *(token + 1) == "DATA" ) {
						if(!PC)
						{
							text_first = false;
							section_text = false;	
						}
						if(section_text)
						{
							end_first_section = PC;
						}
						section_text = false;	
					} else {
						section_text = false;	
					}
				}
            }
//TOKEN � um s�mbolo qualquer (refer�ncia a labels)
            else if(isSymbol(*token))
            {
				if(is_inst) {
					rinst->arg_list.push_back(*token);
				}

            }
            token++;
            PC+=increment_add; /**Nao pode contar diretivas**/
        }
        linha++;

}

code_t PARSER::run_macros(code_t code)
{
    PARSER p;
    code = p.passagem_macros(code);
    return code;
}

vector<int> PARSER::run_montador(code_t code)
{
    PARSER p;
	vector<int> obj_code;
    obj_code = p.passagem_unica(code);
    return obj_code;
}
int PARSER::define_exists(define_t procura)
{
    vector<define_t>::iterator define;
    if(!defines_list.empty())
    {
        define = defines_list.begin();
        while(define!=defines_list.end())
        {
            if(*define == procura)
            {
                return OK;
            }
            define++;
        }
    }
    return !OK;
}

int PARSER::symbol_exists(string procura)
{
    for (int i = 0; i < (int) simb_list.size(); ++i)
    {
        if (simb_list[i].simb == procura)
        {
            return i;
        }
    }
    return -1;
}

int PARSER::islabel(string _label)
{
		size_t found=_label.find(':');
		if(found && _label[found+1] == '\0')
			return OK;
		return !OK;
	return !OK;
}

int PARSER::iscomment(string _comment)
{
    size_t found;
    if((found = _comment.find(';')!=string::npos))
        return OK;
    return !OK;
}
int PARSER::isdir(string _dir)
{
    if(_dir == diretivas::SECTION ||
            _dir == diretivas::TEXT    ||
            _dir == diretivas::DATA    ||
            _dir == diretivas::IF      ||
            _dir == diretivas::EQU     ||
            _dir == diretivas::END     ||
            _dir == diretivas::CONST   ||
            _dir == diretivas::SPACE   ||
            _dir == diretivas::MACRO)
    {
        return OK;
    }
    return !OK;
}

bool PARSER::isSymbol(string simb)
{
    string valid = "ABCDEFGHIJKLMNOPQRSTUVXYZ_0987654321";
    string beg_valid = valid.substr(0,26);
    int i = 0;
    if (beg_valid.find(simb[0]) ==  string::npos)
    {
        return false;
    }
    else
    {
        for (i = 0; i < (int) simb.size(); ++i)
        {
            if (valid.find(simb[i]) == string::npos)
            {
                return false;
            }
        }
    }
    return true;
}

bool PARSER::isNumber(string simb)
{
    string valid = "0987654321";
	string my(simb);
    int i = 0;
	if(my[0] == '-') {
		my = simb.substr(1, simb.length());
	
	}
    for (i = 0; i < (int) my.size(); ++i)
    {
        if (valid.find(my[i]) == string::npos)
        {
            return false;
        }
    }
    return true;
}

bool PARSER::isinst(string _inst,inst_t* rinst)
{
    bool achei = false;
    vector<string>::iterator inst_cur=instruction_list.begin();
    while(inst_cur!=instruction_list.end() && !achei)
    {
        if(*inst_cur == _inst)
        {
			if(rinst != NULL) {
				delete rinst;
			}

            rinst = new inst_t(*inst_cur);
            achei = true;
        }
        inst_cur++;
    }
    return achei;
}

std::string PARSER::ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

string PARSER::translate(inst_t* rinst) {
	string new_code = PARSER::translation[rinst->name];
	vector<string>::iterator arg_it;
	for (int i = 0; i < rinst->arg_list.size(); i++) {
		new_code = ReplaceAll(new_code, "_L"+i+1, (*arg_it));
	}
	return new_code;
}

