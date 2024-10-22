typedef struct{
    u64 Size;
    s8 * Literal;
} identifier; 

typedef struct{
    u32 Id;
} keyword; 

typedef struct{
    s32 Number;
} number_literal; 

typedef struct{
    u32 Enum;
    
    union{
        identifier Identifier;
        keyword Keyword;
        number_literal NumberLiteral;
    };
} token;

typedef enum{
    TOKEN_TYPE_INVALID,

    TOKEN_TYPE_SEMICOLON,
    TOKEN_TYPE_EQU,
    
    TOKEN_TYPE_OPEN_BRACE,
    TOKEN_TYPE_CLOSE_BRACE,
    
    TOKEN_TYPE_OPEN_PARENTHESIS,
    TOKEN_TYPE_CLOSE_PARENTHESIS,
    
    TOKEN_TYPE_MIN,
    TOKEN_TYPE_ADD,
    TOKEN_TYPE_MUL,
    TOKEN_TYPE_DIV,
    
    TOKEN_TYPE_GREATER,
    TOKEN_TYPE_GREATER_EQU,
    TOKEN_TYPE_LESS,
    TOKEN_TYPE_LESS_EQU,
    
    TOKEN_TYPE_LOG_EQU,
    TOKEN_TYPE_LOG_NEQ,
    TOKEN_TYPE_LOG_AND,
    TOKEN_TYPE_LOG_OR,
    TOKEN_TYPE_LOG_NEGATION,
    
    TOKEN_TYPE_BIT_COMPLEMENT,
    TOKEN_TYPE_BIT_AND,
    TOKEN_TYPE_BIT_OR,
    //TOKEN_TYPE_,

    TOKEN_TYPE_NUMBER_LITERAL,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_IDENTIFIER,

    TOKEN_TYPE_COUNT,
} token_type;

#define CaseEnumToString(Enum) case Enum: return #Enum

char * TokenTypeToString(u32 Enum){
    switch(Enum){
        CaseEnumToString(TOKEN_TYPE_INVALID);
        
        CaseEnumToString(TOKEN_TYPE_SEMICOLON);
        CaseEnumToString(TOKEN_TYPE_EQU);
        
        CaseEnumToString(TOKEN_TYPE_OPEN_BRACE);
        CaseEnumToString(TOKEN_TYPE_CLOSE_BRACE);
        
        CaseEnumToString(TOKEN_TYPE_OPEN_PARENTHESIS);
        CaseEnumToString(TOKEN_TYPE_CLOSE_PARENTHESIS);
        
        CaseEnumToString(TOKEN_TYPE_MIN);
        CaseEnumToString(TOKEN_TYPE_ADD);
        CaseEnumToString(TOKEN_TYPE_MUL);
        CaseEnumToString(TOKEN_TYPE_DIV);
        
        CaseEnumToString(TOKEN_TYPE_GREATER);
        CaseEnumToString(TOKEN_TYPE_GREATER_EQU);
        CaseEnumToString(TOKEN_TYPE_LESS);
        CaseEnumToString(TOKEN_TYPE_LESS_EQU);
        
        CaseEnumToString(TOKEN_TYPE_BIT_COMPLEMENT);
        CaseEnumToString(TOKEN_TYPE_BIT_AND);
        CaseEnumToString(TOKEN_TYPE_BIT_OR);
        
        CaseEnumToString(TOKEN_TYPE_LOG_AND);
        CaseEnumToString(TOKEN_TYPE_LOG_OR);
        CaseEnumToString(TOKEN_TYPE_LOG_EQU);
        CaseEnumToString(TOKEN_TYPE_LOG_NEQ);
        CaseEnumToString(TOKEN_TYPE_LOG_NEGATION);
        
        
        CaseEnumToString(TOKEN_TYPE_NUMBER_LITERAL);
        CaseEnumToString(TOKEN_TYPE_KEYWORD);
        CaseEnumToString(TOKEN_TYPE_IDENTIFIER);
        //CaseEnumToString(TOKEN_TYPE_);
        default: return 0;
    }
}

typedef struct{
    char * Literal;
    u32 Enum;
} valid_symbol;

valid_symbol ValidSymbols[] = {
    { ";", TOKEN_TYPE_SEMICOLON },
    { "=", TOKEN_TYPE_EQU },
    { "{", TOKEN_TYPE_OPEN_BRACE },
    { "}", TOKEN_TYPE_CLOSE_BRACE },
    
    { "(", TOKEN_TYPE_OPEN_PARENTHESIS },
    { ")", TOKEN_TYPE_CLOSE_PARENTHESIS },
    
    { "-", TOKEN_TYPE_MIN },
    { "+", TOKEN_TYPE_ADD },
    { "*", TOKEN_TYPE_MUL },
    { "/", TOKEN_TYPE_DIV },
    
    { ">", TOKEN_TYPE_GREATER },
    { "<", TOKEN_TYPE_LESS },
    { ">=", TOKEN_TYPE_GREATER_EQU },
    { "<=", TOKEN_TYPE_LESS_EQU },
    
    { "~", TOKEN_TYPE_BIT_COMPLEMENT },
    { "&", TOKEN_TYPE_BIT_AND },
    { "|", TOKEN_TYPE_BIT_OR },
    
    
    { "!", TOKEN_TYPE_LOG_NEGATION },
    { "&&", TOKEN_TYPE_LOG_AND },
    { "||", TOKEN_TYPE_LOG_OR },
    { "!=", TOKEN_TYPE_LOG_NEQ },
    { "==", TOKEN_TYPE_LOG_EQU },
};

s8 * ValidKeywords[] = {
    "int", //Maybe make a separate growing array for types?
    
    "return",
};

token Tokenize(s8* Literal, u64 Lenght){
    const s32 LiteralCount = sizeof(ValidSymbols) / sizeof(valid_symbol);
    const s32 KeyWordCount = sizeof(ValidKeywords) / sizeof(s8*);

    token Token;
    ZeroMemory(Token);

    u64 StrippedLenght = 0;
    for(int i = 0; i < Lenght; i++){
        if(Literal[i] != ' ' && Literal[i] != '\n' && Literal[i] != '\r'  && Literal[i] != 9){
            StrippedLenght++;
        }
    }
    if(!StrippedLenght){
        return Token;
    }
    
    s8* StrippedLiteral = MemAlloc(StrippedLenght);
    
    s32 Index = 0;
    for(int i = 0; i < Lenght; i++){
        if(Literal[i] != ' ' && Literal[i] != '\n' && Literal[i] != '\r'  && Literal[i] != 9){
            StrippedLiteral[Index++] = Literal[i];
        }
    }
    printf("%s\n", StrippedLiteral);

    b32 HasEnded = !IsAlphabetical(Literal[Lenght]) && !IsNumber(Literal[Lenght]);
    for(int i = 0; i < LiteralCount; i++){
        HasEnded &= !StringCompare(&StrippedLiteral[StrippedLenght], ValidSymbols[i].Literal);
        if(HasEnded) break;
    }

    for(int i = 0; i < LiteralCount; i++){
        if(StringCompare(StrippedLiteral, ValidSymbols[i].Literal)){
            Token.Enum = ValidSymbols[i].Enum;
            return Token;
        }
    }
    
    if(HasEnded){
        if(StrippedLenght > 1){
            for(int i = 0; i < KeyWordCount; i++){
                if(StringCompare(StrippedLiteral, ValidKeywords[i])){
                    Token.Enum = TOKEN_TYPE_KEYWORD;
                    Token.Keyword.Id = i;
                    return Token;
                }
            }
        }
        
        Token.Enum = TOKEN_TYPE_NUMBER_LITERAL;
        for(int i = 0; i < StrippedLenght; i++){
            if(!IsNumber(StrippedLiteral[i])){
                Token.Enum = TOKEN_TYPE_IDENTIFIER;
                Token.Identifier.Literal = StrippedLiteral;
                Token.Identifier.Size = StrippedLenght;
                return Token;
            }
        }
        Token.NumberLiteral.Number = atoi(StrippedLiteral);
    }

    if(Token.Enum == TOKEN_TYPE_INVALID){
        free(StrippedLiteral);
    }
    
    return Token;
}

static b32 TokenIsBinaryOperator(u32 Enum){
    switch(Enum){
        case TOKEN_TYPE_ADD: return 1;
        case TOKEN_TYPE_MIN: return 1;
        case TOKEN_TYPE_MUL: return 1;
        case TOKEN_TYPE_DIV: return 1;
        
        case TOKEN_TYPE_GREATER: return 1;
        case TOKEN_TYPE_GREATER_EQU: return 1;
        case TOKEN_TYPE_LESS: return 1;
        case TOKEN_TYPE_LESS_EQU: return 1;
        case TOKEN_TYPE_LOG_EQU: return 1;
        case TOKEN_TYPE_LOG_NEQ: return 1;
        
        case TOKEN_TYPE_LOG_AND: return 1;
        case TOKEN_TYPE_LOG_OR: return 1;
    }
    return 0;
}

static void TokenPrint(token Token){
    printf("%s", TokenTypeToString(Token.Enum));
    switch(Token.Enum){
        case TOKEN_TYPE_NUMBER_LITERAL:{
            printf(": %i", Token.NumberLiteral.Number);
        }break;
        case TOKEN_TYPE_KEYWORD:{
            printf(": %s", ValidKeywords[Token.Keyword.Id]);
        }break;
        case TOKEN_TYPE_IDENTIFIER:{
            printf(": %s", Token.Identifier.Literal);
        }break;
    }
}
