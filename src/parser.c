typedef enum{
    NODE_TYPE_INVALID,

    NODE_TYPE_FUNCTION,
    NODE_TYPE_ID,
    NODE_TYPE_STATEMENT,
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_OPERATOR,
    
    NODE_TYPE_,
    
    NODE_TYPE_COUNT,
} node_type; 

typedef enum{
    STATEMENT_TYPE_INVALID,

    STATEMENT_TYPE_RETURN,
    
    STATEMENT_TYPE_,
    
    STATEMENT_TYPE_COUNT
} statement_type; 

typedef enum{
    FACTOR_TYPE_INVALID,

    FACTOR_TYPE_EXPRESSION,
    FACTOR_TYPE_UNARY_OP,
    FACTOR_TYPE_NUMBER,

    FACTOR_TYPE_COUNT,
} factor_type; 


typedef enum{
    EXPRESSION_TYPE_INVALID,

    EXPRESSION_TYPE_FACTOR,
    EXPRESSION_TYPE_OPERATOR,

    EXPRESSION_TYPE_COUNT,
} expression_type; 


typedef enum{
    UNARY_OPERATOR_TYPE_INVALID,
    
    UNARY_OPERATOR_TYPE_NEGATION,
    UNARY_OPERATOR_TYPE_LOG_NEGATION,
    UNARY_OPERATOR_TYPE_BIT_COMPLEMENT,
    
    UNARY_OPERATOR_TYPE_COUNT,
} unary_operator_type; 


typedef enum{
    BINARY_OPERATOR_TYPE_INVALID,
    
    BINARY_OPERATOR_TYPE_ADD,
    BINARY_OPERATOR_TYPE_MIN,
    BINARY_OPERATOR_TYPE_MUL,
    BINARY_OPERATOR_TYPE_DIV,
    
    BINARY_OPERATOR_TYPE_COUNT,
} binary_operator_type; 

typedef struct ast_node ast_node;
struct ast_node;


typedef struct factor factor;
struct factor;
struct term;

typedef struct{
    u32 Enum;
    ast_node * Left;
    ast_node * Right;
} binary_operator;

typedef struct{
    u32 Enum;
    factor * Factor;
} unary_operator;

struct factor{
    u32 Enum;
    union{
        s32 Number;
        unary_operator UnOp;
        ast_node * Expression;
    };
};

typedef struct{
    u32 Enum;
    union{
        factor Factor;
        binary_operator Operator;
    };
} expression;
  
typedef struct{
    u32 Enum;
    union{
        struct{
            ast_node * Expression;
        }Return;
    };
} statement;
  
typedef struct{
    identifier Id;
    ast_node * Statement;
} function;

struct ast_node {
    u32 Enum;
    union{
        function Function;
        statement Statement;
        expression Expression;
    };
};

typedef enum{
    AST_STATE_FINE = 0,
    AST_STATE_ERROR = 1,

    AST_STATE_COUNT,
} ast_state;

typedef struct{
    ast_node ** Functions;
    //void * Memory;
    
    s32 State;
    s32 Offset;
    token * Tokens;
} ast;

typedef struct{
    u32 Enum;
    s32 Precedence;
} binary_op_precedence;

binary_op_precedence Precedences[] = {
    {TOKEN_TYPE_MUL, 50},
    {TOKEN_TYPE_DIV, 50},
    {TOKEN_TYPE_ADD, 45},
    {TOKEN_TYPE_MIN, 45},
    
    {TOKEN_TYPE_GREATER, 35},
    {TOKEN_TYPE_GREATER_EQU, 35},
    {TOKEN_TYPE_LESS, 35},
    {TOKEN_TYPE_LESS_EQU, 35},
    {TOKEN_TYPE_LOG_EQU, 30},
    {TOKEN_TYPE_LOG_NEQ, 30},
    
    {TOKEN_TYPE_LOG_AND, 10},
    {TOKEN_TYPE_LOG_OR, 10},
};

s32 BinaryOperatorGetPrecedence(u32 Enum){
    s32 PrecendesCount = sizeof(Precedences) / sizeof(Precedences[0]);
    for(int i = 0; i < PrecendesCount; i++){
        if(Precedences[i].Enum == Enum){
            return Precedences[i].Precedence;
        }
    }
    return -1;
}

void * AstMemAllocate(ast* Ast, u64 Size){
    return MemAlloc(Size);
}

inline b32 ParseCheckNextToken(u32 Enum, ast* Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    return Ast->Tokens[Ast->Offset].Enum == Enum;
}

 
inline token ParseGetNextToken(ast * Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    return Ast->Tokens[Ast->Offset++];
}

 
inline void ParsePrevToken(ast * Ast){
    Assert(Ast->Offset - 1 < 0, "Tokens list size exceded");
    Ast->Offset--;
}

inline void ParseNextToken(ast * Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    Ast->Offset++;
}


inline token ParsePeekNextToken(ast * Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    return Ast->Tokens[Ast->Offset];
}


inline token ParseExpectNextToken(u32 Enum, ast * Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    token Token = Ast->Tokens[Ast->Offset++];
    if(Token.Enum != Enum){
        printf("Error got: ");
        TokenPrint(Token);
        printf(" expected: %s", TokenTypeToString(Enum));
        Ast->State = AST_STATE_ERROR;
    }
    return Token;
}


ast_node * ParseExp(ast* Ast, s32 MinPrecendence);


binary_operator ParseBinaryOperator(ast * Ast, token Token){
    binary_operator Operator;
    ZeroMemory(Operator);

    switch(Token.Enum){
        case TOKEN_TYPE_MIN:{
            Operator.Enum = BINARY_OPERATOR_TYPE_MIN;
        }break; 
        case TOKEN_TYPE_ADD:{
            Operator.Enum = BINARY_OPERATOR_TYPE_ADD;
        }break;
        case TOKEN_TYPE_MUL:{
            Operator.Enum = BINARY_OPERATOR_TYPE_MUL;
        }break;
        case TOKEN_TYPE_DIV:{
            Operator.Enum = BINARY_OPERATOR_TYPE_DIV;
        }break;
        default:{
            Operator.Enum = BINARY_OPERATOR_TYPE_INVALID;
        }break;
    }
            
    if(Operator.Enum == BINARY_OPERATOR_TYPE_INVALID){
        //error log
    }
    
    return Operator;
}

unary_operator ParseUnaryOperator(ast * Ast, token Token){
    unary_operator Operator;
    ZeroMemory(Operator);

    switch(Token.Enum){
        case TOKEN_TYPE_MIN:{
            Operator.Enum = UNARY_OPERATOR_TYPE_NEGATION;
        }break; 
        case TOKEN_TYPE_LOG_NEGATION:{
            Operator.Enum = UNARY_OPERATOR_TYPE_LOG_NEGATION; 
        }break;
        case TOKEN_TYPE_BIT_COMPLEMENT:{
            Operator.Enum = UNARY_OPERATOR_TYPE_BIT_COMPLEMENT; 
        }break;
        default:{
            Operator.Enum = UNARY_OPERATOR_TYPE_INVALID;
        }break;
    }
            
    if(Operator.Enum == UNARY_OPERATOR_TYPE_INVALID){
        //error log
    }
    
    return Operator;
}

factor ParseFactor(ast * Ast){
    factor Factor;
    ZeroMemory(Factor);

    token Token = ParseGetNextToken(Ast);
    switch(Token.Enum){
        case TOKEN_TYPE_OPEN_PARENTHESIS:{
            ast_node * Expr = ParseExp(Ast, 0);
            if(Expr){
                Factor.Enum = FACTOR_TYPE_EXPRESSION;
                Factor.Expression = Expr;
            }
            ParseExpectNextToken(TOKEN_TYPE_CLOSE_PARENTHESIS, Ast);
        }break;
        
        case TOKEN_TYPE_NUMBER_LITERAL:{
            Factor.Enum = FACTOR_TYPE_NUMBER;
            Factor.Number = Token.NumberLiteral.Number;
        }break;
        
        default:{
            unary_operator UnOp = ParseUnaryOperator(Ast, Token);
            if(UnOp.Enum != UNARY_OPERATOR_TYPE_INVALID){
                UnOp.Factor = AstMemAllocate(Ast, sizeof(factor));
                *UnOp.Factor = ParseFactor(Ast);
                if(UnOp.Factor){
                    Factor.Enum = FACTOR_TYPE_UNARY_OP;
                    Factor.UnOp = UnOp;
                }
            }
            
        }break;

    }
    
    return Factor;
    
}

ast_node * ParseExp(ast* Ast, s32 MinPrecendence){
    ast_node* Node = 0;
    expression Expression;
    ZeroMemory(Expression);

    Expression.Enum = EXPRESSION_TYPE_FACTOR;
    Expression.Factor = ParseFactor(Ast);
    
    token Token = ParsePeekNextToken(Ast);
    s32 Precedence = BinaryOperatorGetPrecedence(Token.Enum);
    while(TokenIsBinaryOperator(Token.Enum) && Precedence >= MinPrecendence)
    {
        binary_operator BiOp = ParseBinaryOperator(Ast, Token);
        ParseNextToken(Ast);
        BiOp.Right = ParseExp(Ast, Precedence + 1);
        
        Node = AstMemAllocate(Ast, sizeof(ast_node));
        Node->Enum = NODE_TYPE_EXPRESSION;
        Node->Expression = Expression;
        BiOp.Left = Node;
    
        Expression.Enum = EXPRESSION_TYPE_OPERATOR;
        Expression.Operator = BiOp;
        
        Token = ParsePeekNextToken(Ast);
        Precedence = BinaryOperatorGetPrecedence(Token.Enum);
    }
    
    
    if(Ast->State == AST_STATE_ERROR){
        return 0;
    }
    
    Node = AstMemAllocate(Ast, sizeof(ast_node));
    Node->Enum = NODE_TYPE_EXPRESSION;
    Node->Expression = Expression;
    
    return Node;
}

ast_node *ParseStatement(ast* Ast){
    statement Statement;
    ZeroMemory(Statement);

    token Token = ParseExpectNextToken(TOKEN_TYPE_KEYWORD, Ast);
    switch(Token.Keyword.Id){
        case 0:{
                
        }break;
            
        case 1:{
            Statement.Enum = STATEMENT_TYPE_RETURN;
            Statement.Return.Expression = ParseExp(Ast, 0);
        }break;
            
        default: return 0;
    }
    ParseExpectNextToken(TOKEN_TYPE_SEMICOLON, Ast);

    if(Ast->State == AST_STATE_ERROR){
        return 0;
    }
    
    ast_node* Node = AstMemAllocate(Ast, sizeof(ast_node));
    Node->Enum = NODE_TYPE_STATEMENT;
    Node->Statement = Statement;
    
    return Node;
}

ast_node * ParseFunction(ast* Ast){
    token RetType = ParseExpectNextToken (TOKEN_TYPE_KEYWORD, Ast);
    token FuncId = ParseExpectNextToken(TOKEN_TYPE_IDENTIFIER, Ast);
    ParseExpectNextToken(TOKEN_TYPE_OPEN_PARENTHESIS, Ast);
    ParseExpectNextToken(TOKEN_TYPE_CLOSE_PARENTHESIS, Ast);
    ParseExpectNextToken(TOKEN_TYPE_OPEN_BRACE, Ast);
    
    ast_node * Statement = ParseStatement(Ast);
    if(!Statement) return 0;
    
    ParseExpectNextToken(TOKEN_TYPE_CLOSE_BRACE, Ast);

    if(Ast->State == AST_STATE_ERROR){
        return 0;
    }
    
    ast_node* Node = AstMemAllocate(Ast, sizeof(ast_node));
    Node->Enum = NODE_TYPE_FUNCTION;
    Node->Function.Statement = Statement;
    Node->Function.Id = FuncId.Identifier;
    
    return Node;
}

ast AstGenerate(token * Tokens, u64 MemorySize){
    ast Ast;
    ZeroMemory(Ast);
    Ast.Tokens = Tokens;
    Ast.Functions = ArrayAlloc(ast_node*);
    for(; Ast.Offset < ArraySize(Tokens); Ast.Offset++){
        token Token = Ast.Tokens[Ast.Offset];
        
        switch(Token.Enum){
            case TOKEN_TYPE_KEYWORD:{
                switch(Token.Keyword.Id){
                    case 0:{
                        ast_node * Function =  ParseFunction(&Ast);
                        if(Function){
                            ArrayAppend(Ast.Functions, Function);
                        }
                    }break;
                }
            }break;
        }
    }

    return Ast;
}


