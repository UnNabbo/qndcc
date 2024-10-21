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
    EXPRESSION_TYPE_INVALID,

    EXPRESSION_TYPE_NUMBER_LITERAL,
    EXPRESSION_TYPE_OPERATION,

    EXPRESSION_TYPE_COUNT,
} expression_type; 

typedef enum{
    OPERATOR_TYPE_INVALID,
    
    OPERATOR_TYPE_UNARY,
    OPERATOR_TYPE_BINARY,
    OPERATOR_TYPE_,
    
    OPERATOR_TYPE_COUNT,
} operator_type; 

typedef enum{
    UNARY_OPERATOR_TYPE_INVALID,
    
    UNARY_OPERATOR_TYPE_NEGATION,
    UNARY_OPERATOR_TYPE_LOG_NEGATION,
    UNARY_OPERATOR_TYPE_BIT_COMPLEMENT,
    
    UNARY_OPERATOR_TYPE_COUNT,
} operator_type; 


typedef struct ast_node ast_node;
struct ast_node;

typedef struct{
    u32 Enum;
    ast_node * Expression;
} unary_operator;

typedef struct{
    u32 Enum;
    union{
        unary_operator UnaryOperator; 
    };
    
} operator;

typedef struct{
    u32 Enum;
    union{
        s32 Number;
        operator Operator;
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

ast_node * AstNodeAllocate(ast* Ast){
    return MemAlloc(sizeof(ast_node));
}

inline b32 ParseCheckNextToken(u32 Enum, ast* Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    TokenPrint(Ast->Tokens[Ast->Offset]);
    return Ast->Tokens[Ast->Offset].Enum == Enum;
}

 
inline token ParseGetNextToken(ast * Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    return Ast->Tokens[Ast->Offset++];
}

inline token ParseExpectNextToken(u32 Enum, ast * Ast){
    Assert(Ast->Offset + 1 >= ArraySize(Ast->Tokens), "Tokens list size exceded");
    token Token = Ast->Tokens[Ast->Offset++];
    if(Token.Enum != Enum){
        printf("Error got: ");
        TokenPrint(Token);
        Ast->State = AST_STATE_ERROR;
    }
    return Token;
}


ast_node * ParseExp(ast* Ast);

operator ParseOperator(u32 Kind, ast * Ast){
    operator Operator;
    ZeroMemory(Operator);
    
    Operator.Enum = Kind;

    token Token = ParseGetNextToken(Ast);
    TokenPrint(Token);
    switch(Kind){
        case OPERATOR_TYPE_UNARY:{
            switch(Token.Enum){
                case TOKEN_TYPE_NEGATION:{
                    Operator.UnaryOperator.Enum = UNARY_OPERATOR_TYPE_NEGATION;
                }break; 
                case TOKEN_TYPE_LOG_NEGATION:{
                    Operator.UnaryOperator.Enum = UNARY_OPERATOR_TYPE_LOG_NEGATION; 
                }break;
                case TOKEN_TYPE_BIT_COMPLEMENT:{
                    Operator.UnaryOperator.Enum = UNARY_OPERATOR_TYPE_BIT_COMPLEMENT; 
                }break;
                default:{
                    Operator.Enum = OPERATOR_TYPE_INVALID;
                }break;
            }
            
            if(Operator.Enum != OPERATOR_TYPE_INVALID){
                Operator.UnaryOperator.Expression = ParseExp(Ast);
                if(!Operator.UnaryOperator.Expression){
                    Operator.Enum = OPERATOR_TYPE_INVALID;
                }
            }
        }break;
    }
    return Operator;
}

ast_node * ParseExp(ast* Ast){
    expression Expression;
    ZeroMemory(Expression);
    
    if(ParseCheckNextToken(TOKEN_TYPE_NUMBER_LITERAL, Ast)){
        token Token = ParseGetNextToken(Ast);
        Expression.Enum = EXPRESSION_TYPE_NUMBER_LITERAL;
        Expression.Number = Token.NumberLiteral.Number;
    }else{
        Expression.Operator = ParseOperator(OPERATOR_TYPE_UNARY, Ast);
        Expression.Enum = EXPRESSION_TYPE_OPERATION;
        if(Expression.Operator.Enum == OPERATOR_TYPE_INVALID){
            return 0;
        }
    }

    if(Ast->State == AST_STATE_ERROR){
        return 0;
    }
    
    ast_node* Node = AstNodeAllocate(Ast);
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
            Statement.Return.Expression = ParseExp(Ast);
        }break;
            
        default: return 0;
    }
    ParseExpectNextToken(TOKEN_TYPE_SEMICOLON, Ast);

    if(Ast->State == AST_STATE_ERROR){
        return 0;
    }
    
    ast_node* Node = AstNodeAllocate(Ast);
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
    
    ast_node* Node = AstNodeAllocate(Ast);
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


