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

typedef struct term term;
struct term;

typedef struct{
    u32 Enum;
    union{
        term * Term;
        factor * Factor;
    };
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

typedef struct binary_op_chain binary_op_chain;
struct binary_op_chain{
    binary_operator Operator;
    
    binary_op_chain * Next;
};

struct term{
    factor Factor;
    
    binary_op_chain * Factors;
} ;

typedef struct{
    term Term;
    
    binary_op_chain * Terms;
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


ast_node * ParseExp(ast* Ast);

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
            ast_node * Expr = ParseExp(Ast);
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

term ParseTerm(ast * Ast){
    term Term;
    ZeroMemory(Term);

    Term.Factor = ParseFactor(Ast);

    binary_op_chain * Prev = 0;
    while(1){
        token Token = ParsePeekNextToken(Ast);
        if(Token.Enum != TOKEN_TYPE_MUL && Token.Enum != TOKEN_TYPE_DIV) break;
        binary_operator Operator = ParseBinaryOperator(Ast, Token);
        ParseNextToken(Ast);
        
        if(Operator.Enum == BINARY_OPERATOR_TYPE_INVALID){
            //logging;
            break;
        }
        
        factor Factor = ParseFactor(Ast);
        if(Factor.Enum == FACTOR_TYPE_INVALID){
            //logging;
            break;
        }
               
        binary_op_chain * Chain = AstMemAllocate(Ast, sizeof(binary_op_chain));
        Chain->Operator = Operator;
        
        if(!Term.Factors){
            Term.Factors = Chain;
        }
        
        if(Prev){
            Prev->Next = Chain;
        }
            Chain->Operator.Factor = AstMemAllocate(Ast, sizeof(term));
            *Chain->Operator.Factor = Factor;
        Prev = Chain;
    }

    
    return Term;
}

ast_node * ParseExp(ast* Ast){
    expression Expression;
    ZeroMemory(Expression);

    Expression.Term = ParseTerm(Ast);
    binary_op_chain * Prev = 0;
    while(1){
        token Token = ParsePeekNextToken(Ast);
        if(Token.Enum != TOKEN_TYPE_ADD && Token.Enum != TOKEN_TYPE_MIN) break;
        binary_operator Operator = ParseBinaryOperator(Ast, Token);
        ParseNextToken(Ast);
        
        if(Operator.Enum == BINARY_OPERATOR_TYPE_INVALID){
            //logging;
            break;
        }
        
        term Term = ParseTerm(Ast);
        if(Term.Factor.Enum == FACTOR_TYPE_INVALID){
            printf("Term is invalid");
            break;
        }
               
        binary_op_chain * Chain = AstMemAllocate(Ast, sizeof(binary_op_chain));
        Chain->Operator = Operator;
        
        if(!Expression.Terms){
            Expression.Terms = Chain;
        }
        
        if(Prev){
            Prev->Next = Chain;
        }
        
        Chain->Operator.Term = AstMemAllocate(Ast, sizeof(term));
        *Chain->Operator.Term = Term;
        Prev = Chain;
    }

    if(Ast->State == AST_STATE_ERROR){
        return 0;
    }
    
    ast_node* Node = AstMemAllocate(Ast, sizeof(ast_node));
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


