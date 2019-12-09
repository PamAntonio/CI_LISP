%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbolTableNode;
}

%token <sval> FUNC SYMBOL TYPE
%token <dval> INT DOUBLE
%token LPAREN RPAREN EOL QUIT LET COND LAMBDA

%type <astNode> s_expr f_expr number s_expr_list
%type <symbolTableNode> let_section let_list let_element arg_list

%%
program:
    EOL {
        fprintf(stderr, "yacc: program ::= EOL\n");
    }
    | s_expr EOL {
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
    | f_expr {
        $$ = $1;
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | SYMBOL {
        fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
        $$ = createSymbolNode($1);
    }
    | LPAREN let_section s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= ( let_section s_expr ) \n");
        $$ = linkSymbolTable( $2, $3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= ( cond s_expr s_expr s_expr ) \n");
        $$ = createConditionalNode($3,$4,$5);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

let_section:
    LPAREN LET let_list RPAREN{
        $$ = $3;
    };

let_list:
    let_element {
        $$ = $1;
    }
    | let_list let_element {
        $$ = addAtHead($1,$2);
    };

let_element:
    LPAREN TYPE SYMBOL s_expr RPAREN {
        $$ = createFuncNode($2, $3, NULL, $4);
    }
    | LPAREN SYMBOL s_expr RPAREN {
        $$ = createFuncNode(NULL, $2, NULL, $3);
    }
    | LPAREN SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
        $$ = createFuncNode(NULL, $2, $5, $7);
    }
    | LPAREN TYPE SYMBOL LAMBDA LPAREN arg_list RPAREN s_expr RPAREN {
        $$ = createFuncNode($2, $3, $6, $8);
    };

arg_list:
    SYMBOL arg_list {
        $$ = linkArgList($1, $2);
    }
    | SYMBOL {
        $$ = linkArgList($1, NULL);
    };

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

s_expr_list:
    s_expr s_expr_list {
        $$ = linkFunNodeList($1,$2);
    }
    | s_expr {
        $$ = $1;
    };

f_expr:
    LPAREN SYMBOL s_expr_list RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN SYMBOL s_expr_list\n");
        $$ = createFunctionNode($2, $3);
    }
    | LPAREN FUNC RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC RPAREN\n");
        $$ = createFunctionNode($2, NULL);
    }
    | LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$ = createFunctionNode($2, $3);
    }
    | LPAREN FUNC s_expr_list RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr_list RPAREN\n");
        $$ = createFunctionNode($2, $3);
    };
%%
