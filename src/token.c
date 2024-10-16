typedef struct{
    u64 Size;
    s8 * Literal;
} identifier; 

typedef struct{
    u64 Id;
} keyword; 

typedef struct{
    u64 Number;
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
    TOKEN_TYPE_OPEN_BRACE,
    TOKEN_TYPE_CLOSE_BRACE,
    TOKEN_TYPE_OPEN_PARENTHESIS,
    TOKEN_TYPE_CLOSE_PARENTHESIS,

    TOKEN_TYPE_NUMBER_LITERAL,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_IDENTIFIER,

    TOKEN_TYPE_,
    
    TOKEN_TYPE_COUNT,
} token_type;

s8 ValidTokenLiteral[] = {
    ';',
    '{',
    '}',
    '(',
    ')',
};

s8 * ValidKeywords[] = {
    "int",
    "return",
};

token Tokenize(s8* Literal, u64 Lenght){
    const s32 LiteralCount = sizeof(ValidTokenLiteral) / sizeof(s8);
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

    if(StrippedLenght == 1){
        for(int i = 0; i < LiteralCount; i++){
            if(StrippedLiteral[0] == ValidTokenLiteral[i]){
                Token.Enum = i + 1;
                return Token;
            }
        }
    }

    b32 HasEnded = !IsAlphabetical(Literal[Lenght]) && !IsNumber(Literal[Lenght]);
    for(int i = 0; i < LiteralCount; i++){
        HasEnded &= StrippedLiteral[StrippedLenght] != ValidTokenLiteral[i];
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


static void TokenPrint(token Token){
    switch(Token.Enum){
        case TOKEN_TYPE_SEMICOLON:{
            printf("TOKEN_TYPE_SEMICOLON");
        }break;

        case TOKEN_TYPE_OPEN_BRACE:{
            printf("TOKEN_TYPE_OPEN_BRACE");
        }break;
            
        case TOKEN_TYPE_CLOSE_BRACE:{
            printf("TOKEN_TYPE_CLOSE_BRACE");
        }break;

        case TOKEN_TYPE_OPEN_PARENTHESIS:{
            printf("TOKEN_TYPE_OPEN_PARENTHESIS");
        }break;
        case TOKEN_TYPE_CLOSE_PARENTHESIS:{
            printf("TOKEN_TYPE_CLOSE_PARENTHESIS");
        }break;

        case TOKEN_TYPE_NUMBER_LITERAL:{
            printf("TOKEN_TYPE_NUMBER_LITERAL: %llu", Token.NumberLiteral.Number);
        }break;
        case TOKEN_TYPE_KEYWORD:{
            printf("TOKEN_TYPE_KEYWORD: %s", ValidKeywords[Token.Keyword.Id]);
        }break;
        case TOKEN_TYPE_IDENTIFIER:{
            printf("TOKEN_TYPE_IDENTIFIER: ");
            for(int j = 0; j < Token.Identifier.Size; j++){
                putchar(Token.Identifier.Literal[j]);
            }
        }break;
    }
    printf("\n");
}
