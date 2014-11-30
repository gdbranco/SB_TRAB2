#include "parser.h"
vector<string> PARSER::instruction_list;
map<string, string> PARSER::translation;
string PARSER::retiraComentarios(string _linha)
{
    size_t found = _linha.find(';');
    if(found!=string::npos)
    {
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
    instruction_list.push_back(instructions::C_INPUT);
    instruction_list.push_back(instructions::C_OUTPUT);
    instruction_list.push_back(instructions::S_INPUT);
    instruction_list.push_back(instructions::S_OUTPUT);
    instruction_list.push_back(instructions::INPUT);
    instruction_list.push_back(instructions::OUTPUT);
    instruction_list.push_back(instructions::STOP);

	translation["STOP"] = "mov eax, 1\nmov ebx, 0\nint 0x80";

	translation["C_INPUT"] = "mov eax, 3\nmov ebx, 0\nmov ecx, _L1\nmov edx, 1\nint 0x80";

	translation["C_OUTPUT"] = "mov eax, 4\nmov ebx, 1\nmov ecx, _L1\nmov edx, 1\nint 0x80";

	translation["S_INPUT"] = "push _L2\npush _L1\ncall __read_string";

	translation["S_OUTPUT"] = "push _L1\ncall __print_string";

	translation["INPUT"] = "mov eax, 3\nmov ebx, 0\nmov ecx, _L1\nmov edx, 1\nint 0x80\nsub word [_L1], 0x30";

	translation["OUTPUT"] = "add word [_L1], 0x30\nmov eax, 4\nmov ebx, 1\nmov ecx, _L1\nmov edx, 1\nint 0x80\nsub word [_L1], 0x30";

	translation["JMP"] = "jmp _L1";

	translation["JMPZ"] = "cmp ax, 0\nje _L1";

	translation["JMPP"] = "cmp ax, 0\njg _L1";

	translation["JMPN"] = "cmp ax, 0\njl _L1";

	translation["LOAD"] = "mov ax, [_L1]";

	translation["STORE"] = "mov [_L1], ax";

	translation["ADD"] = "add word ax, [_L1]";

	translation["SUB"] = "sub word ax, [_L1]";

	translation["MULT"] = "mul word [_L1]";

	translation["DIV"] = "xor dx, dx\ndiv word [_L1]";

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
            /**Checa se a linha eh um IF e se a proxima é valida**/
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
                    if(*token == diretivas::IF) //Essa condicao é inutil, retirar depois
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
		
        /*Se a linha possui "macro", é uma declaração que deve ser colocada na tabela.*/
        if (std::find(it->tokens.begin(), it->tokens.end(), "MACRO") != it->tokens.end())
        {
			/*Guarda o indice do iterador para recuperá-lo após apagar do vetor*/
			int aux = it - code.begin();
			m_beg = it;
			m_end = mct.create_macro(it, code);
			if(m_end != code.end())
			{
				code.erase(m_beg, m_end + 1);
			}
			it = code.begin() + aux -1;


            /*Caso contrário, confere se possui label e declaração e substitui o código.*/
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
string PARSER::passagem_unica(code_t code)
{
    unsigned int PC=0;
    int sinal;
    code_t _code = code;
	code_t const_lines;
    tsum_t sum_list;
	vector<inst_t> instrucoes;
	vector<inst_t> s_read_list;
	vector<inst_t>::iterator inst_it;
	vector<space_t> space_code;
	vector<unsigned int> string_index_list;
    code_t::iterator linha = _code.begin();
    tsmb_t::iterator last_symbol;
    unsigned int increment_add;
    bool space_found, const_found, is_soma, has_label;
	bool section_text = true;
	bool is_inst = false;
	int end_first_section = 0;
	bool text_first=true;
	bool no_label = false;
	inst_t *rinst = NULL;
	string text_ia32 = "";
	string num_space = "";
	string n = "1"; 

    while(linha!=_code.end())
    {
		rinst = new inst_t("");
        space_found = false;
        const_found = false;
        is_soma = false;
		is_inst = false;
		has_label = false;
		no_label = false;
		n = "1";

        sinal = 1;
        vector<string>::iterator token = linha->tokens.begin();

		if (find(linha->tokens.begin(), linha->tokens.end(), "SPACE") != linha->tokens.end() ) {
			no_label = true;
		}

        while(token!=linha->tokens.end())
        {
//TOKEN == +
            if ((*token) == "+")
            {
				/*Confere se a expressão acaba prematuramente*/
				is_soma = true;
				increment_add = 0;
				if(inst_it->arg_list.size() > 0) {
					inst_it->arg_list[inst_it->arg_list.size() - 1] += " + ";
				}	
            }
//TOKEN == -
            else if ((*token) == "-")
            {
				is_soma = true;
				sinal = -1;
				increment_add = 0;
				if(inst_it->arg_list.size() > 0) {
					inst_it->arg_list[inst_it->arg_list.size() - 1] += " - ";
				}	
            }
//TOKEN anterior é + ou -
            else if (is_soma)
            {
                increment_add = 0;
				/*Se é numero direto, soma*/
                if (isNumber(*token))
                {
					int aux = atoi(token->c_str());
					aux *= 2;
					if(inst_it->arg_list.size() > 0) {
						inst_it->arg_list[inst_it->arg_list.size() - 1] += to_string(aux);
					}	
                }
            }
//TOKEN anterior é SPACE
            else if(space_found)
			{
				if(token!=linha->tokens.end())
				{
					if (isNumber(*token)) {
						increment_add = atoi(token->c_str());
						increment_add--;

					}
				}
				n = linha->tokens.at(2);
				linha->tokens.at(0).pop_back();
			}
//TOKEN anterior é CONST
            else if(const_found)
            {
				text_ia32.pop_back();
				text_ia32 += *token + "\n";
				//const_code.push_back(const_t(linha->tokens.at(0), linha->tokens.at(2)));
            }
//TOKEN é uma label
            else if(islabel(*token))
            {
					has_label = true;
					if(!no_label) {
						text_ia32 += *token + " ";
					}
            }
//TOKEN é uma instrução
            else if(isinst(*token,rinst)) /**Refazer para melhorar a estrutura de instrucoes e diretivas**/
            {
				is_inst = true;
				instrucoes.push_back(*rinst);
				inst_it = instrucoes.end() - 1;
            }
//TOKEN é uma diretiva
            else if(isdir(*token))
            {
                increment_add=0;
                if(*token == diretivas::SPACE)
                {
                    space_found = true;
                    increment_add=1;

                } else if(*token == diretivas::CONST) {
					const_found = true;
					text_ia32 += "dw 1";
				}
//TOKEN é uma secao
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

						text_ia32 += "section .text\nglobal _start\n";
						text_ia32 += nasm_functions::__read_string;
						text_ia32 += nasm_functions::__print_string;
						text_ia32 += "\n\n";
						text_ia32 += "_start:\n";

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
						text_ia32 += "section .data\n";
						text_ia32 += "__ERROR_MSG: db \"Error: Tentativa de acesso a memoria nao reservada\",0ah\n";
						text_ia32 += "__ERROR_MSG_SIZE: EQU $-__ERROR_MSG\n";

						section_text = false;	
					} else {
						section_text = false;	
					}
				}
            }
//TOKEN é um símbolo qualquer (referência a labels)
            else if(isSymbol(*token))
            {
				if(is_inst) {
					if(inst_it->name == instructions::S_INPUT) {
						inst_it->arg_list.push_back(*token);

						s_read_list.push_back(*inst_it);
						string_index_list.push_back(text_ia32.size());

						is_inst = false;
					} else {
						inst_it->arg_list.push_back(*token);
					}
				}

            }
            token++;
            PC+=increment_add; /**Nao pode contar diretivas**/
        }
		if(space_found)
		{	
				space_code.push_back(space_t(linha->tokens.at(0), n));
		}
        linha++;

		if(is_inst) {
			text_ia32 += translate(&(instrucoes.back()));
			text_ia32 += "\n";
		}
	}

	if (!space_code.empty()) {
		text_ia32 += "section .bss\n";

		for (unsigned int i = 0; i < space_code.size(); i++) {
			text_ia32 += space_code[i].label + " resw " + space_code[i].num + "\n";
		}
	}

_str:
	int s_ind = 0;
	string aux = "";
	if (!s_read_list.empty()) {
		for(auto it = s_read_list.begin(); it != s_read_list.end(); it++) {
			aux = "";
			num_space = "";
			for(auto sp_it = space_code.begin(); sp_it != space_code.end(); sp_it++) {
				if(sp_it->label == it->arg_list[0]) {
					num_space = sp_it->num;		
					break;
				}
			}
			it->arg_list.push_back(num_space);
			aux += translate(&(*it));
			aux += "\n";
			text_ia32.insert(string_index_list[s_ind], aux);

			s_ind++;
		}	
	}
	
	//cout << text_ia32;

	space_code.erase(space_code.begin(),space_code.end());
	instruction_list.erase(instruction_list.begin(),instruction_list.end());
	delete rinst;
	return text_ia32;
}

code_t PARSER::run_macros(code_t code)
{
    PARSER p;
    code = p.passagem_macros(code);
    return code;
}

string PARSER::run_montador(code_t code)
{
    PARSER p;
    return  p.passagem_unica(code);
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

            rinst->name = *inst_cur;
			rinst->arg_list.clear();
            achei = true;
			break;
        }
        inst_cur++;
    }
    return achei;
}

std::string PARSER::ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); 
    }
    return str;
}

string PARSER::translate(inst_t* rinst) 
{
	string new_code = PARSER::translation[rinst->name];
	string aux = "_L1";
	for (unsigned int i = 0; i < rinst->arg_list.size(); i++) {
		aux = "_L"+ to_string(i+1);
		new_code = ReplaceAll(new_code, aux, rinst->arg_list[i]);
	}
	return new_code;
}

