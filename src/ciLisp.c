// Deanne Pamela Antonio
// ciLisp.c Task 1 - 9 done
//


#include "ciLisp.h"


void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""
};

char *numTypeNames[] = {
        "int",
        "double",
        ""
};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0'){
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}


NUM_TYPE resolveNum(char *numName){
    int i = 0;
    while (numTypeNames[i][0] != '\0'){
        if (strcmp(numTypeNames[i], numName) == 0){
            free(numName);
            return i;
        }
        i++;
    }
    return NO_TYPE;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type){
    AST_NODE *node = newNode(NUM_NODE_TYPE);

    switch (type){
        case INT_TYPE:
            node->data.number.type = INT_TYPE;
            node->data.number.value.ival = (long) value;
            break;
        case DOUBLE_TYPE:
            node->data.number.type = DOUBLE_TYPE;
            node->data.number.value.dval = value;
            break;
        default:
            break;
        }
    return node;
}

AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1)
{
    AST_NODE *node = newNode(FUNC_NODE_TYPE);
    node->data.function.oper = resolveFunc(funcName);

    if (node->data.function.oper == CUSTOM_OPER)
        node->data.function.ident = funcName;
    else
        free(funcName);

    node->data.function.opList = op1;
    AST_NODE *currOp = op1;

    while (currOp != NULL){
        currOp->parent = node;
        currOp = currOp->next;
    }

    return node;
}

AST_NODE *createSymbolNode(char *ident){
    AST_NODE *node = newNode(SYMBOL_NODE_TYPE);
    node->data.symbol.ident = ident;

    return node;
}

AST_NODE *sExprLink(AST_NODE *newNode, AST_NODE *head)
{
    newNode->next = head;
    return newNode;
}

AST_NODE *astLink(SYMBOL_TABLE_NODE *letList, AST_NODE *op)
{
    op->symbolTable = letList;
    SYMBOL_TABLE_NODE *node = letList;

    while (node != NULL){
        node->val->parent = op;
        node = node->next;
    }

    return op;
}

SYMBOL_TABLE_NODE *createSymbolTableNode(AST_NODE *val, char *type, char *ident){
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed");

    node->ident = ident;
    node->val = val;
    node->next = NULL;
    node->sym_type = VARIABLE_TYPE;
    node->val_type = resolveNum(type);

    return node;
}

SYMBOL_TABLE_NODE *letLink(SYMBOL_TABLE_NODE *head, SYMBOL_TABLE_NODE *newVal){
    newVal->next = head;
    return newVal;
}

AST_NODE *createCondition(AST_NODE *condExp, AST_NODE *nonZero, AST_NODE *zero){
    AST_NODE *node = newNode(COND_NODE_TYPE);

    node->data.condition.condNode = condExp;
    condExp->parent = node;
    node->data.condition.nonZero = nonZero;
    nonZero->parent = node;
    node->data.condition.zero = zero;
    zero->parent = node;

    return node;
}

ARG_TABLE_NODE *createArgTable(char *headName, ARG_TABLE_NODE *list){
    ARG_TABLE_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(ARG_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed");

    node->ident = headName;
    node->next = list;
    node->argVal = (RET_VAL) {DOUBLE_TYPE, NAN};

    return node;
}

SYMBOL_TABLE_NODE *createLambdaSymbolTableNode(AST_NODE *val, char *type, char *ident, ARG_TABLE_NODE *argList)
{
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed");

    node->ident = ident;
    node->val_type = resolveNum(type);
    node->next = NULL;
    node->val = val;

    node->sym_type = LAMBDA_TYPE;
    val->argTable = argList;

    return node;
}


AST_NODE *newNode(AST_NODE_TYPE type){
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed");

    node->type = type;
    node->parent = NULL;
    node->next = NULL;
    node->symbolTable = NULL;
    node->argTable = NULL;

    return node;
}

void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    AST_NODE *currOp;
    switch (node->type)
    {
        case NUM_NODE_TYPE:
            break;

        case FUNC_NODE_TYPE:
            currOp = node->data.function.opList;
            while (currOp != NULL){
                freeNode(currOp);
                currOp = currOp->next;
            }

            if (node->data.function.oper == CUSTOM_OPER) {
                free(node->data.function.ident);
            }
            break;

        case SYMBOL_NODE_TYPE:
            free(node->data.symbol.ident);
            break;

        case COND_NODE_TYPE:
            freeNode(node->data.condition.condNode);
            freeNode(node->data.condition.nonZero);
            freeNode(node->data.condition.zero);
            break;
    } 

    SYMBOL_TABLE_NODE *currNode = node->symbolTable;
    SYMBOL_TABLE_NODE *prevNode;

    while (currNode !=NULL)
    {
        prevNode = currNode;
        currNode = currNode->next;

        free(prevNode->ident);
        free(prevNode);
    }

    ARG_TABLE_NODE *currArg = node->argTable;
    ARG_TABLE_NODE *prevArg;

    while (currArg !=NULL)
    {
        prevArg = currArg;
        currArg = currArg->next;

        free(prevArg->ident);
        free(prevArg);
    }

    free(node);
}

RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN}; 

    switch (node->type)
    {
        case FUNC_NODE_TYPE:
            result = evalFuncNode(node);
            break;
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case SYMBOL_NODE_TYPE:
            result = evalSymbolNode(node);
            break;
        case COND_NODE_TYPE:
            result = evalCondNode(&node->data.condition);
            break;
        default:
            yyerror("Invalid ast node type");
    }
    return result;
}


RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN};

    result.type = numNode->type;
    switch (numNode->type)
    {
        case INT_TYPE:
            result.value.ival = numNode->value.ival;
            break;
        case DOUBLE_TYPE:
            result.value.dval = numNode->value.dval;
            break;
        default:
            yyerror("Invalid num node type");
    }


    return result;
}


RET_VAL evalFuncNode(AST_NODE *node){
    if (!node)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    FUNC_AST_NODE *funcNode = &(node->data.function);

    RET_VAL result = {DOUBLE_TYPE, NAN};

    switch (funcNode->oper){


        case NEG_OPER:
            if (!funcNode->opList)
                break;

            result = eval(funcNode->opList);
            switch (result.type)
            {
                case INT_TYPE:
                    result.value.ival = -result.value.ival;
                    break;
               case DOUBLE_TYPE:
                    result.value.dval = -result.value.dval;
                    break;
               default:
                    yyerror("Invalid num node type");
            }   

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"neg\".\n\t\tOther param ignored\n");
            break;


        case ABS_OPER:
            if (!funcNode->opList)
               break;

            result = eval(funcNode->opList);
            switch (result.type){
                case INT_TYPE:
                    result.value.ival = labs(result.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.value.dval = fabs(result.value.dval);
                    break;
                default:
                    yyerror("Invalid num node type");
            }

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"abs\".\n\t\tOther param ignored\n");
            break;


        case EXP_OPER:
            if (!funcNode->opList)
               break;
            
            result = eval(funcNode->opList);
            switch (result.type){
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = exp((double)result.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.value.dval = exp(result.value.dval);
                    break;
                default:
                    yyerror("Invalid num node type");
            }    

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"exp\".\n\t\tOther param ignored\n");
            break;

        case SQRT_OPER:
            if (!funcNode->opList)
               break;

            result = eval(funcNode->opList);
            switch (result.type){
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = sqrt( (double) result.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.value.dval = sqrt(result.value.dval);
                    break;
                default:
                    yyerror("Invalid num node type");
            }     

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"sqrt\".\n\t\tOther param ignored\n");
            break;

        case ADD_OPER:
            if (!funcNode->opList)
               break;
            else if (!funcNode->opList->next){
                yyerror("Too few parameters for \"add\".\n");
                break;
            }

            result = eval(funcNode->opList);
            AST_NODE *currOp = funcNode->opList->next;
            RET_VAL op2; 
        
            while (currOp != NULL)
            {
                op2 = eval(currOp);
                switch (result.type) {
                    case INT_TYPE:
                        switch (op2.type) {
                            case INT_TYPE:
                                result.value.ival += op2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.type = DOUBLE_TYPE;
                                result.value.dval = (double)result.value.ival + op2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    case DOUBLE_TYPE:
                        switch (op2.type) {
                            case INT_TYPE:
                                result.value.dval += (double) op2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.value.dval += op2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    default:
                        yyerror("Invalid num noded type");
                } 
                currOp = currOp->next;
            } 
            break;

        case SUB_OPER:
            //result = helperSubOper(funcNode->opList);
            if (!funcNode->opList)
               break;

            else if (!funcNode->opList->next){
                yyerror("Too few parameters for \"sub\".\n");
                break;
            }

            result = eval(funcNode->opList);
            AST_NODE *subCurrOp = funcNode->opList->next;
            RET_VAL subOp2; 
        
            while (subCurrOp != NULL){
                subOp2 = eval(subCurrOp);
                switch (result.type) {
                    case INT_TYPE:
                        switch (subOp2.type) {
                            case INT_TYPE:
                                result.value.ival -= subOp2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.type = DOUBLE_TYPE;
                                result.value.dval = (double)result.value.ival - subOp2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    case DOUBLE_TYPE:
                        switch (subOp2.type) {
                            case INT_TYPE:
                                result.value.dval -= (double) subOp2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.value.dval -= subOp2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    default:
                        yyerror("Invalid num noded type");
                } 
                subCurrOp = subCurrOp->next;
            } 
            break;

        case MULT_OPER:
            if (!funcNode->opList)
               break;
            
            else if (!funcNode->opList->next){
                yyerror("Too few parameters for \"mult\".\n");
                break;
            }

            result = eval(funcNode->opList);
            AST_NODE *multCurrOp = funcNode->opList->next;
            RET_VAL multOp2; 
        
            while (multCurrOp != NULL){
                multOp2 = eval(multCurrOp);
                switch (result.type) {
                    case INT_TYPE:
                        switch (multOp2.type) {
                            case INT_TYPE:
                                result.value.ival *= multOp2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.type = DOUBLE_TYPE;
                                result.value.dval = (double)result.value.ival * multOp2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    case DOUBLE_TYPE:
                        switch (multOp2.type) {
                            case INT_TYPE:
                                result.value.dval *= (double) multOp2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.value.dval *= multOp2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    default:
                        yyerror("Invalid num node type");
                } 
                multCurrOp = multCurrOp->next;
            } 
            
            break;

        case DIV_OPER:
            if (!funcNode->opList)
               break;

            else if (!funcNode->opList->next){
                yyerror("Too few parameters for \"div\".\n");
                break;
            }

            result = eval(funcNode->opList);
            AST_NODE *divCurrOP = funcNode->opList->next;
            RET_VAL divOp2; 
        
            while (divCurrOP != NULL){
                divOp2 = eval(divCurrOP);
                switch (result.type) {
                    case INT_TYPE:
                        switch (divOp2.type) {
                            case INT_TYPE:
                                result.value.ival /= divOp2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.type = DOUBLE_TYPE;
                                result.value.dval = (double)result.value.ival / divOp2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    case DOUBLE_TYPE:
                        switch (divOp2.type) {
                            case INT_TYPE:
                                result.value.dval /= (double) divOp2.value.ival;
                                break;
                            case DOUBLE_TYPE:
                                result.value.dval /= divOp2.value.dval;
                                break;
                            default:
                                yyerror("Invalid num node type");
                        }
                        break;
                    default:
                        yyerror("Invalid num node type");
                } 
                divCurrOP = divCurrOP->next;
            } 
            break;

        case REMAINDER_OPER:
            if (!funcNode->opList)
               break;

            else if (!funcNode->opList->next){
                yyerror("Too few parameters for \"remainder\".\n");
                break;
            }

            result = eval(funcNode->opList);
            RET_VAL remOp2 = eval(funcNode->opList->next);

            switch (result.type){
                case INT_TYPE:
                    switch (remOp2.type){
                        case INT_TYPE:
                            result.value.ival = result.value.ival % remOp2.value.ival;
                            break;
                        case DOUBLE_TYPE:
                            result.type = DOUBLE_TYPE;
                            result.value.dval = fmod((double) result.value.ival, remOp2.value.dval);
                            break;
                        default:
                            yyerror("Invalid num node type");
                    }
                    break;
                case DOUBLE_TYPE:
                    switch (remOp2.type){
                        case INT_TYPE:
                            result.value.dval = fmod(result.value.dval, (double) remOp2.value.ival);
                            break;
                        case DOUBLE_TYPE:
                            result.value.dval = fmod(result.value.dval, remOp2.value.dval);
                            break;
                        default:
                            yyerror("Invalid num node type");
                    }
                    break;
                default:
                    yyerror("Invalid num node type");
            }

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"remainder\".\n\t\tOther param ignored\n");

            break;


        case LOG_OPER:
            if (!funcNode->opList)
               break;

            result = eval(funcNode->opList);
            switch (result.type){
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = log((double)result.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.value.dval = log(result.value.dval);
                    break;
                default:
                    yyerror("Invalid num node type");
            }     

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"log\".\n\t\tOther param ignored\n");
            break;

        case POW_OPER:
            if (!funcNode->opList)
               break;

            else if (!funcNode->opList->next){
                yyerror("Too few parameters for \"pow\".\n");
                break;
            }            

            result = eval(funcNode->opList);
            RET_VAL powOp2 = eval(funcNode->opList->next);

            switch (result.type){
                case INT_TYPE:
                    switch (powOp2.type)
                    {
                        case INT_TYPE:
                            result.value.ival = lround(pow( (double) result.value.ival, (double) powOp2.value.ival));
                            break;
                        case DOUBLE_TYPE:
                            result.type = DOUBLE_TYPE;
                            result.value.dval = pow((double) result.value.ival, powOp2.value.dval);
                            break;
                        default:
                            yyerror("Invalid num node type");
                    }
                    break;
                case DOUBLE_TYPE:
                    switch (powOp2.type){
                        case INT_TYPE:
                            result.value.dval = pow(result.value.dval, (double) powOp2.value.ival);
                            break;
                        case DOUBLE_TYPE:
                            result.value.dval = pow( result.value.dval, powOp2.value.dval );
                            break;
                        default:
                            yyerror("Invalid NUM_NODE_TYPE, probably invalid writes somewhere!");
                    }
                    break;
                default:
                    yyerror("Invalid NUM_NODE_TYPE, probably invalid writes somewhere!");
            }

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"pow\".\n\t\tOther param ignored\n");
            break;

        case MAX_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"max\".\n\t\tOther param ignored\n");

            break;

        case MIN_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"min\".\n\t\tOther param ignored\n");

            break;

        case EXP2_OPER:
            if (!funcNode->opList)
               break;
            

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"exp2\".\n\t\tOther param ignored\n");
            break;

        case CBRT_OPER:
            if (!funcNode->opList)
               break;
            

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"cbrt\".\n\t\tOther param ignored\n");
            break;

        case HYPOT_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"hypot\".\n\t\tOther param ignored\n");

            break;

        case PRINT_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"print\".\n\t\tOther param ignored\n");

            break;

        case READ_OPER:
            if (!funcNode->opList)
               break;
            

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"read\".\n\t\tOther param ignored\n");
            break;

        case RAND_OPER:
            if (!funcNode->opList)
               break;
            

            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"rand\".\n\t\tOther param ignored\n");

            break;

        case EQUAL_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"equal\".\n\t\tOther param ignored\n");

            break;

        case LESS_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"less\".\n\t\tOther param ignored\n");

            break;

        case GREATER_OPER:
            if (!funcNode->opList)
               break;
            
            if (funcNode->opList->next != NULL)
                yyerror("Too many parameters for \"greater\".\n\t\tOther param ignored\n");

            break;

        case CUSTOM_OPER:
            if (!funcNode->opList)
               break;
            


            break;
        default:
            printf("What?");
            break;
    }

    return result;
}

RET_VAL evalSymbolNode(AST_NODE *symbolNode)
{

    if (!symbolNode)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN};

    char *symbol = symbolNode->data.symbol.ident;
    AST_NODE *currNode = symbolNode;
    SYMBOL_TABLE_NODE *currSymbol;
    ARG_TABLE_NODE *currArg;


    while (currNode != NULL){
        currSymbol = currNode->symbolTable;
        while (currSymbol != NULL){
            if (!strcmp(symbol, currSymbol->ident) && (currSymbol->sym_type == VARIABLE_TYPE)){
                result = evalSymbolNodeHelper(currSymbol);

                return result;
            }
            currSymbol = currSymbol->next;
        } 

        currArg = currNode->argTable;
        while (currArg != NULL){
            if (!strcmp(symbol, currArg->ident))
            {
                result = currArg->argVal;

                return result;
            }

            currArg = currArg->next;
        } 


        currNode = currNode->parent;
    } 

    return result;
}


RET_VAL evalSymbolNodeHelper(SYMBOL_TABLE_NODE *symbol){
    if (!symbol)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = eval(symbol->val);
    switch (symbol->val_type){
        case NO_TYPE:
            return result;
        case INT_TYPE:
            switch (result.type){
                case INT_TYPE:
                    break;
                case DOUBLE_TYPE:
                    printf("WARNING: precision loss for assignment \'%s\'\n", symbol->ident);
                    result.type = INT_TYPE;
                    result.value.ival = lround(result.value.dval);
                    break;
                default:
                    yyerror("Invalid num node type");
            }
            break;

        case DOUBLE_TYPE:
            switch (result.type)
            {
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = (double) result.value.ival;
                    break;
                case DOUBLE_TYPE:
                    break;
                default:
                    yyerror("Invalid num node type");
            }
            break;

        default:
            yyerror("Invalid num node type");
    } 
    return result;
}


RET_VAL evalCondNode(COND_AST_NODE *condAstNode){
    if (!condAstNode)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = eval(condAstNode->condNode);

    switch (result.type){
        case INT_TYPE:
            if (result.value.ival)
                result = eval(condAstNode->nonZero);
            else
                result = eval(condAstNode->zero);
            break;
        case DOUBLE_TYPE:
            if (result.value.dval)
                result = eval(condAstNode->nonZero);
            else
                result = eval(condAstNode->zero);
            break;
        default:
            yyerror("Invalid NUM_NODE_TYPE, probably invalid writes somewhere!");
            break;
    }
    return result;
}


void printRetVal(RET_VAL val){
    switch (val.type){
        case INT_TYPE:
            printf("Type: Integer \n%ld\n", val.value.ival);
            break;
        case DOUBLE_TYPE:
            printf("Type: Double \n%lf\n", val.value.dval);
            break;
        default:
            yyerror("Invalid num node type");
    }
}


void attachStackNodes(ARG_TABLE_NODE *lambdaArgs, STACK_NODE *paramVals){
    ARG_TABLE_NODE *currArg = lambdaArgs;
    STACK_NODE *currStackNode = paramVals;
    STACK_NODE *prevStackNode;

    while ((currArg != NULL)){
        prevStackNode = currStackNode;
        currArg->argVal = currStackNode->val;
        currArg = currArg->next;
        currStackNode = currStackNode->next;
        free(prevStackNode);
    }
}