SECTION TEXT
resto_macro: macro
store RESTO
output RESTO
end
teste: macro
load QUOT
store B + 1
end


s_input teste
s_output teste
input B + 1
loop:
load B + 1
div DOIS
store QUOT
load QUOT
mult DOIS
store VOLTA
load B + 1
sub VOLTA
resto_macro
teste
jmpp loop

stop

SECTION DATA
B: SPACE 2
DOIS: CONST 2
QUOT: SPACE
resto: SPACE
volta: space
teste: space 10
