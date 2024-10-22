
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Assert(cond, msg, ...) if(!cond){ printf(msg "\n", __VA_ARGS__); exit(1);}

#undef ZeroMemory
#define ZeroMemory(var) memset(&var, 0, sizeof(var))
#define MemAlloc(size) calloc(1, size)
#define MemRealloc(ptr, size) realloc(ptr, size)
#define MemFree(ptr) free(ptr)
#define MemCopy(dest, src, size) memcpy(dest, src, size)

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef float              f32;
typedef double             f64;
typedef unsigned char      b8;
typedef unsigned char      b16;
typedef unsigned int       b32;
typedef unsigned long long b64;

#include "utils.c"
#include "array.c"
#include "file.c"

#include "token.c"
#include "parser.c"
#include "code_gen.c"

#include "compile_options.c"
#include "ast_preatty_print.c"

void Compile(char * Path, compile_options Options){
    char *Input = FileOpenAndRead(Path);
    printf("%s\n", Input);

    char * OutputPath = Options.AsmPath;
    if(!OutputPath){
       
    }
    
    char Format[1024];
    ZeroMemory(Format);
    
    char * Name = PathExtractName(Path);
    s32 NameLength = strlen(Name);
    
    token* Tokens = ArrayAlloc(token);
    u32 TokenSize = 0;
    u32 TokenOffset = 0;
    
    for(int i = 0; Input[i]; i++){
        TokenSize++;
        token Token = Tokenize(Input + TokenOffset, TokenSize);
        if(Token.Enum != TOKEN_TYPE_INVALID){
            TokenOffset += TokenSize; 
            TokenSize = 0;
            
            ArrayAppend(Tokens, Token);
        }
    }

    ast Ast = AstGenerate(Tokens, 0);
    if(Ast.State == AST_STATE_FINE){
        sprintf(Format, "%s%s.s", OutputPath, Name);
        AssemblyGenerate(&Ast, Format);
    
        sprintf(Format, "gcc %s%s.s -o %s%s.exe", OutputPath, Name, OutputPath, Name);
        ExecuteCommand(Format);
    }


    #if 1
    printf("\n");
    For(Token, Tokens){
        TokenPrint(Token);
    printf("\n");
    }
    
    //printf("\n");
    //AstPrint(&Ast);
    #endif


    
    MemFree(Input);
    MemFree(Name);
    ArrayFree(Tokens);

}

int main(s32 Argc, char * Argv[]) 
{
    compile_options Options;
    ZeroMemory(Options);

    char** Files = FilesAndOptionsFind(&Options, Argv, Argc);
    for(int i = 0; i < ArraySize(Files); i++){
        Compile(Files[i], Options);
    }
    ArrayFree(Files);
    
	return 0;
}
