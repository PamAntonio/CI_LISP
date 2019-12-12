%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symNode;
    struct arg_table_node *argNode;
}

%token <sval> FUNC SYMBOL TYPE
%token <dval> INT DOUBLE
%token LPAREN RPAREN LET COND LAMBDA EOL QUIT

%type <astNode> s_expr f_expr number s_expr_list
%type <symNode> let_elem let_section let_list
%type <argNode> arg_list


%%

program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    number {
        fprintf(stderr, "yacc: s_expr ::= number\n");
        $$ = $1;
    }
    | SYMBOL {
    	fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
	$$ = createSymbolNode($1);
    }
    | f_expr {
	fprintf(stderr, "yacc: s_expr ::= f_expr\n");
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    }
    | LPAREN let_section s_expr RPAREN {
    	fprintf(stderr, "yacc: s_expr ::= LPAREN let_section s_expr RPAREN\n");
    	$$ = astLink($2, $3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {
    	fprintf(stderr, "yacc: s_expr ::= LPAREN COND s_expr s_expr s_expr RPAREN\n");
    	$$ = createCondition ($3, $4, $5);
    }
    | LPAREN s_expr RPAREN {
	fprintf(stderr, "yacc: s_expr ::= LPAREN s_expr RPAREN\n");
        $$ = $2;
    };

s_expr_list:
    s_expr s_expr_list {
    	fprintf(stderr, "yacc: s_expr_list ::= s_expr s_expr_list\n");
    	$$ = sExprLink($1, $2);
    }
    | s_expr {
        fprintf(stderr, "yacc: s_expr_list ::= s_expr\n");
	$$ = $1;
    }

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };
    | TYPE INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($2, resolveNum($1));
    }
    | TYPE DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($2, resolveNum($1));
    };

let_section:
    LPAREN let_list RPAREN {
    	fprintf(stderr, "yacc: let_section ::= LPAREN let_list RPAREN\n");
    	$$ = $2;
    };

let_list:
    LET let_elem {
      	fprintf(stderr, "yacc: let_list ::= LET let_elem\n");
    	$$ = $2;
    }
    | let_list let_elem {
    	fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
    	$$ = letLink($1, $2);
    }

let_elem:
    LPAREN SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($3, "", $2);
    }
    | LPAREN TYPE SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN TYPE SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($4, $2, $3);
    }
    | LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
        $$ = createLambdaSymbolTableNode($7, "", $2, $5);
    }
    | LPAREN TYPE SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN TYPE SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN\n");
        $$ = createLambdaSymbolTableNode($8, $2, $3, $6);
    };

arg_list:
    SYMBOL arg_list {
    	fprintf(stderr, "yacc: arg_list ::= SYMBOL arg_list\n");
	$$ = createArgTable($1, $2);
    }
    | SYMBOL {
    	fprintf(stderr, "yacc: arg_list ::= SYMBOL\n");
	$$ = createArgTable($1, NULL);
    }

f_expr:
    LPAREN FUNC s_expr_list RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC s_expr RPAREN\n");
        $$ = createFunctionNode($2, $3);
    }
    | LPAREN FUNC RPAREN {
    	fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC RPAREN\n");
    	$$ = createFunctionNode($2, NULL);
    }
    | LPAREN SYMBOL s_expr_list RPAREN {
            fprintf(stderr, "yacc: s_expr ::= LPAREN SYMBOL s_expr_list RPAREN\n");
            $$ = createFunctionNode($2, $3);
    }
%%
