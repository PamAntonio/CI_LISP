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

OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (funcNames[i][0] != '\0') {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
//    char *funcs[] = {"neg",
//                     "abs",
//                     "exp",
//                     "sqrt",
//                     "add",
//                     "sub",
//                     "mult",
//                     "div",
//                     "remainder",
//                     "log",
//                     "pow",
//                     "max",
//                     "min",
//                     "exp2",
//                     "cbrt",
//                     "hypot",
//                     "read",
//                     "rand",
//                     "print",
//                     "equal",
//                     "less",
//                     "greater",
//                     "" };
//
//    int i = 0;
//    while (funcs[i][0] !='\0'){
//        if (!strcmp(funcs[i], funcName))
//            return i;
//        i++;
//    }
//    yyerror("invalid function");
//    return -1;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type) {
    AST_NODE *node;
    size_t nodeSize;
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE
    node->type = NUM_NODE_TYPE;
    node->data.number.type = type;
    switch (type) {
        case INT_TYPE:
            node->data.number.value = value;
            break;
        case DOUBLE_TYPE:
            node->data.number.value = value;
            break;
        default:
            break;
    }
    return node;
}

// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
//
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1) {
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = (AST_NODE *) malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.

    node->type = FUNC_NODE_TYPE;

    node->data.function.oper = resolveFunc(funcName);
    if (node->data.function.oper == CUSTOM_OPER) {
        node->data.function.ident = malloc(sizeof(funcName));
        strcpy(node->data.function.ident, funcName);
    }

    node->data.function.opList = op1;

    AST_NODE *head = node->data.function.opList;
    while (head != NULL) {
        head->parent = node;
        head = head->next;
    }
    return node;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node) {
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE) {
        freeNode(node->data.function.opList);
        if (node->data.function.oper == CUSTOM_OPER) {
            free(node->data.function.ident);
        }
    }
    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node) {
    if (!node)
        return (RET_VAL) {INT_TYPE, NAN};
    RET_VAL result = {INT_TYPE, NAN};

    // TODO complete the switch.
    switch (node->type) {
        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            if (node->data.function.oper == READ_OPER || node->data.function.oper == RAND_OPER) {
                node->type = NUM_NODE_TYPE;
                node->data.number = result;
            }
            break;
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case SYMBOL_NODE_TYPE:
            result = evalSymNode(&node->data.symbol, node);
            break;
        case COND_NODE_TYPE:
            result = evalConditionNode(&node->data.condition);
            break;
        default:
            yyerror("Invalid");
    }

    return result;
}

RET_VAL evalSymNode(SYMBOL_AST_NODE *symbolNode, AST_NODE *parent) {
    RET_VAL result = {INT_TYPE, NAN};

    SYMBOL_TABLE_NODE *temp = parent->symbolTable;
    AST_NODE *pnt = parent->parent;
    bool found = false;

    while (!found) {
        if (temp == NULL) { // wasn't found in this scope, go up a scope, look for the var there
            if (parent->parent != NULL) {
                temp = pnt->symbolTable;
                pnt = pnt->parent;
            } else {
                break;
            }
        } else {
            if (strcmp(symbolNode->ident, temp->ident) == 0) {
                result = eval(temp->val);
                if (temp->val_type == INT_TYPE && result.type == DOUBLE_TYPE) {
                    result.value = (int) result.value;
                    result.type = INT_TYPE;
                    printf("Warning: Precision loss for assignment \'%s\'\n", temp->ident);
                }
                if (temp->val_type == DOUBLE_TYPE && result.type == INT_TYPE) {
                    result.type = DOUBLE_TYPE;
                }
                found = true;
            }
            temp = temp->next;
        }
    }

    return result;
}

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode) {
    if (!numNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the values stored in the node.
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE

    result.type = numNode->type;
    switch (numNode->type) {
        case INT_TYPE:
            result.value = numNode->value;
            break;
        case DOUBLE_TYPE:
            result.value = numNode->value;
            break;
    }

    return result;
}

RET_VAL evalConditionNode(COND_AST_NODE *conditionNode) {
    RET_VAL result = {INT_TYPE, NAN};

    RET_VAL cond = eval(conditionNode->cond);
    if (cond.value != 0) {
        result = eval(conditionNode->tru);
    } else {
        result = eval(conditionNode->fls);
    }

    return result;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    int numOfOperands = 0;
    AST_NODE *head = funcNode->opList;
    while (head != NULL) {
        numOfOperands++;
        head = head->next;
    }

    if (funcNode->oper == CUSTOM_OPER) {
        AST_NODE *parent = NULL;
        if (funcNode->opList == NULL) {
            printf("Error\n");
            result.value = 0.0;
        } else {
            parent = funcNode->opList->parent;
        }

        SYMBOL_TABLE_NODE *h = NULL;
        while (parent != NULL) {
            if (parent->symbolTable != NULL) {
                h = parent->symbolTable;
                break;
            }
            parent = parent->parent;
        }
        while (h != NULL) {
            if (h->type == LAMBDA_TYPE) {
                if (strcmp(h->ident, funcNode->ident) == 0) {
                    SYMBOL_TABLE_NODE *arg = h->val->symbolTable;
                    AST_NODE *plist = funcNode->opList;
                    while (arg != NULL) {
                        AST_NODE *argResolved = malloc(sizeof(AST_NODE));
                        argResolved->type = NUM_NODE_TYPE;
                        if (plist == NULL) {
                            printf("Error\n");
                            break;
                        }
                        argResolved->data.number = eval(plist);
                        arg->val = argResolved;
                        arg = arg->next;
                        plist = plist->next;
                        if (arg == NULL && plist != NULL) {
                            printf("Error\n");
                        }
                    }
                    result = eval(h->val);
                    result.type = h->val_type;
                }
            }
            h = h->next;
        }
    } else {
        if (numOfOperands == 0) {
            switch (funcNode->oper) {
                case RAND_OPER:
                    result.type = DOUBLE_TYPE;
                    result.value = ((double) rand()) / RAND_MAX;
                    break;
                case READ_OPER:
                    result = readNum();
                    break;
                default:
                    break;
            }
        }

        if (numOfOperands == 1) {
            result = eval(funcNode->opList);
            switch (funcNode->oper) {
                case NEG_OPER:
                    result.value = -result.value;
                    break;
                case ABS_OPER:
                    result.value = fabs(result.value);
                    break;
                case EXP_OPER:
                    result.value = exp(result.value);
                    break;
                case SQRT_OPER:
                    result.value = sqrt(result.value);
                    break;
                case LOG_OPER:
                    result.value = log(result.value);
                    break;
                case EXP2_OPER:
                    result.value = exp2(result.value);
                    break;
                case CBRT_OPER:
                    result.value = cbrt(result.value);
                    break;
                case PRINT_OPER:
                    printType(result);
                    break;
                default:
                    printf("ERROR: need more parameters: %s", funcNames[funcNode->oper]);
                    break;
            }
        }

        if (numOfOperands >= 2) {
            RET_VAL op1 = {INT_TYPE, NAN};
            RET_VAL op2 = {INT_TYPE, NAN};
            if (numOfOperands == 2) {
                op1 = eval(funcNode->opList);
                op2 = eval(funcNode->opList->next);
                if (op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE) {
                    result.type = DOUBLE_TYPE;
                }
            }
            AST_NODE *h = funcNode->opList;

            if (numOfOperands > 2 &&
                (funcNode->oper == SUB_OPER || funcNode->oper == DIV_OPER || funcNode->oper == REMAINDER_OPER ||
                 funcNode->oper == POW_OPER || funcNode->oper == MAX_OPER || funcNode->oper == MIN_OPER ||
                 funcNode->oper == HYPOT_OPER)) {
                printf("WARNING: too many parameters: %s\n", funcNames[funcNode->oper]);
            }

            switch (funcNode->oper) {
                case SUB_OPER:
                    result.value = op1.value - op2.value;
                    break;
                case DIV_OPER:
                    result.value = op1.value / op2.value;
                    break;
                case REMAINDER_OPER:
                    result.value = fmod(op1.value, op2.value);
                    break;
                case POW_OPER:
                    result.value = pow(op1.value, op2.value);
                    break;
                case MAX_OPER:
                    result.value = fmax(op1.value, op2.value);
                    break;
                case MIN_OPER:
                    result.value = fmin(op1.value, op2.value);
                    break;
                case HYPOT_OPER:
                    result.value = hypot(op1.value, op2.value);
                    break;
                case ADD_OPER:
                    result.value = 0;
                    if (numOfOperands == 2)
                        result.value = op1.value + op2.value;
                    else {
                        while (h != NULL) {
                            RET_VAL temp = eval(h);
                            if (temp.type == DOUBLE_TYPE)
                                result.type = DOUBLE_TYPE;
                            result.value += temp.value;
                            h = h->next;
                        }
                    }
                    break;
                case LESS_OPER:
                    if (op1.value < op2.value)
                        result.value = 1;
                    else
                        result.value = 0;
                    break;
                case GREATER_OPER:
                    if (op1.value > op2.value)
                        result.value = 1;
                    else
                        result.value = 0;
                    break;
                case EQUAL_OPER:
                    if (op1.value == op2.value)
                        result.value = 1;
                    else
                        result.value = 0;
                    break;
                case MULT_OPER:
                    result.value = 1;
                    if (numOfOperands == 2)
                        result.value = op1.value * op2.value;
                    else {
                        while (h != NULL) {
                            RET_VAL temp = eval(h);
                            if (temp.type == DOUBLE_TYPE)
                                result.type = DOUBLE_TYPE;
                            result.value *= temp.value;
                            h = h->next;
                        }
                    }
                    break;
                case PRINT_OPER:
                    if (numOfOperands == 2) {
                        printType(op1);
                        printf("\n");

                        printRetVal(op1);
                        printf("\n");

                        //  printRetVal(op2);
                        printType(op2);
                        result = op2;
                    } else {
                        RET_VAL values[numOfOperands];
                        for (int i = 0; i < numOfOperands; i++) {
                            values[i] = eval(h);
                            h = h->next;
                        }
                        for (int i = 0; i < numOfOperands; i++) {
                            printRetVal(values[i]);
                            printType(values[i]);
                        }
                        result = values[numOfOperands - 1];
                    }
                    printf("\n");
                    break;
                default:
                    break;
            }
        }
    }

    return result;
}

void printRetVal(RET_VAL val) {
    // TODO value of the value passed in.
    switch (val.type) {
        case INT_TYPE:
            printf("%d \n", (int) val.value);
            break;
        case DOUBLE_TYPE:
            printf("%.2f \n", val.value);
            break;
    }
}

void printType(RET_VAL val) {
    // TODO print the type of value passed in.
    switch (val.type) {
        case INT_TYPE:
            printf("Type: Integer\n");
            break;
        case DOUBLE_TYPE:
            printf("Type: Double\n");
            break;
    }
}

AST_NODE *createSymbolNode(char *ident) {
    AST_NODE *symbolNode = malloc(sizeof(SYMBOL_AST_NODE));
    symbolNode->type = SYMBOL_NODE_TYPE;
    symbolNode->parent = NULL;
    symbolNode->symbolTable = NULL;
    symbolNode->data.symbol.ident = malloc(strlen(ident) * sizeof(char));
    strcpy(symbolNode->data.symbol.ident, ident);
    return symbolNode;
}

AST_NODE *linkSymbolTable(SYMBOL_TABLE_NODE *symTable, AST_NODE *node) {
    if (node != NULL)
        node->symbolTable = symTable;
    return node;
}

SYMBOL_TABLE_NODE *addAtHead(SYMBOL_TABLE_NODE *list, SYMBOL_TABLE_NODE *element) {
    element->next = list;
    return element;
}

AST_NODE *linkFunNodeList(AST_NODE *newItem, AST_NODE *list) {
    if (newItem != NULL)
        newItem->next = list;
    return newItem;
}

AST_NODE *createConditionalNode(AST_NODE *condition, AST_NODE *doOne, AST_NODE *doZero) {
    COND_AST_NODE *conditionNode = malloc(sizeof(COND_AST_NODE));
    conditionNode->cond = condition;
    conditionNode->tru = doOne;
    conditionNode->fls = doZero;


    AST_NODE *cN = malloc(sizeof(AST_NODE));
    cN->type = COND_NODE_TYPE;
    cN->data.condition = *conditionNode;
    cN->parent = NULL;
    cN->next = NULL;
    cN->symbolTable = NULL;

    cN->data.condition.cond->parent = cN;
    cN->data.condition.tru->parent = cN;
    cN->data.condition.fls->parent = cN;

    return cN;
}

RET_VAL readNum() {
    printf("read := ");
    char input[10];
    scanf("%s", input);
    getchar();

    char num[10];
    bool decimal = false;
    int i;
    for (i = 0; i < strlen(input); i++) {
        if (!decimal && input[i] == '.') {
            decimal = true;
            num[i] = input[i];
        }
        if (isdigit(input[i]))
            num[i] = input[i];
    }
    if (i == 9)
        num[9] = '\0';
    else
        num[i] = '\0';

    RET_VAL res = {INT_TYPE, NAN};

    if (decimal)
        res.type = DOUBLE_TYPE;
    char **junk = 0;
    res.value = strtod(num, junk);
    return res;
}

SYMBOL_TABLE_NODE *linkArgList(char *addition, SYMBOL_TABLE_NODE *list) {
    SYMBOL_TABLE_NODE *new = malloc(sizeof(SYMBOL_TABLE_NODE));
    new->next = list;
    new->type = ARG_TYPE;
    new->val_type = DOUBLE_TYPE;
    new->ident = malloc(sizeof(strlen(addition)));
    strcpy(new->ident, addition);
    new->val = NULL;
    new->stack = NULL;
    return new;
}

SYMBOL_TABLE_NODE *createFuncNode(char *type, char *identifier, SYMBOL_TABLE_NODE *args, AST_NODE *s_expr) {
    SYMBOL_TABLE_NODE *symbolTableNode = malloc(sizeof(SYMBOL_TABLE_NODE));
    symbolTableNode->ident = malloc(strlen(identifier) * sizeof(char) + 1);
    strcpy(symbolTableNode->ident, identifier);

    if (args == NULL)
        symbolTableNode->type = VARIABLE_TYPE;
    else
        symbolTableNode->type = LAMBDA_TYPE;
    if (type != NULL) {
        if (strcmp(type, "int") == 0)
            symbolTableNode->val_type = INT_TYPE;
        if (strcmp(type, "double") == 0)
            symbolTableNode->val_type = DOUBLE_TYPE;
    } else
        symbolTableNode->val_type = DOUBLE_TYPE;

    symbolTableNode->stack = NULL;
    symbolTableNode->val = s_expr;
    symbolTableNode->val->symbolTable = args;
    symbolTableNode->next = NULL;

    return symbolTableNode;
}
