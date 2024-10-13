typedef enum{
    SYMBOL_TYPE_INVALID,

    SYMBOL_TYPE_FUNCTION,
    SYMBOL_TYPE_ID,
    SYMBOL_TYPE_STATEMENT,
    SYMBOL_TYPE_EXPRESSION,
    
    SYMBOL_TYPE_,
    
    SYMBOL_TYPE_COUNT,
} symbol_type;

typedef struct{
    u32 Enum;
} symbol;

struct ast_node {
    struct ast_node * Parent;
    struct ast_node * Child;
    struct ast_node * Sibling;

    symbol Symbol;
};

struct ast_node;
typedef struct ast_node ast_node;

typedef struct{
    ast_node * Root;
    //void * Memory;
} ast;

ast_node * AstNodeAllocate(ast* Ast){
    return MemAlloc(sizeof(ast_node));
}


token ParseExpectNextToken(u32 Enum, token * Tokens, s32* Offset){
    Assert(Offset >= ArraySize(Tokens), "Tokens list size exceded");
    
    token Token = Tokens[(*Offset)++];
    if(Token.Enum != Enum){
        printf("Error got: ");
        TokenPrint(Token);
    }
    
    return Token;
}

ast_node * ParseExp(ast* Ast, token * Tokens, s32* Offset){
    ast_node* Node = AstNodeAllocate(Ast);
    ParseExpectNextToken(TOKEN_TYPE_NUMBER_LITERAL, Tokens, Offset);
    
    return Node;
}

ast_node *ParseStatement(ast* Ast, token * Tokens, s32* Offset){
    ast_node* Node = AstNodeAllocate(Ast);
    
    token Token = Tokens[(*Offset)++];
    if(Token.Enum != TOKEN_TYPE_KEYWORD){
        if(Token.Keyword.Id != 1){
            printf("Error got: ");
            TokenPrint(Token);
        }
    }
    Node->Child = ParseExp(Ast, Tokens, Offset);
    ParseExpectNextToken(TOKEN_TYPE_SEMICOLON, Tokens, Offset);
    
    Node->Symbol.Enum = SYMBOL_TYPE_STATEMENT;
    return Node;
}

ast_node *ParseFunction(ast* Ast, token * Tokens, s32* Offset){
    ast_node* Node = AstNodeAllocate(Ast);

    token RetType = ParseExpectNextToken(TOKEN_TYPE_KEYWORD, Tokens, Offset);
    token FuncId = ParseExpectNextToken(TOKEN_TYPE_IDENTIFIER, Tokens, Offset);
    ParseExpectNextToken(TOKEN_TYPE_OPEN_PARENTHESIS, Tokens, Offset);
    ParseExpectNextToken(TOKEN_TYPE_CLOSE_PARENTHESIS, Tokens, Offset);
    ParseExpectNextToken(TOKEN_TYPE_OPEN_BRACE, Tokens, Offset);
    Node->Child = ParseStatement(Ast, Tokens, Offset);
    ParseExpectNextToken(TOKEN_TYPE_CLOSE_BRACE, Tokens, Offset);

    Node->Symbol.Enum = SYMBOL_TYPE_FUNCTION;
    return Node;
}

ast Parse(token * Tokens, u64 MemorySize){
    ast Ast;
    ZeroMemory(Ast);
    
    for(int i = 0; i < ArraySize(Tokens); i++){
        token Token = Tokens[i];
        TokenPrint(Token);
        ast_node* Node;
        ZeroMemory(Node);
        
        switch(Token.Enum){
            case TOKEN_TYPE_KEYWORD:{
                switch(Token.Keyword.Id){
                    case 0:{
                        Node = ParseFunction(&Ast, Tokens, &i);
                    }break;
                }
            }break;
        }
        
        
    }
    return Ast;
}

    

