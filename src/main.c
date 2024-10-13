
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

static s32 FileTested = 0;

void Compile(char * Code){
    printf("\nCase %i:\n\n%s\n\n", ++FileTested, Code); 
    
    token* Tokens = ArrayAlloc(token);
  
    u32 TokenSize = 0;
    u32 TokenOffset = 0;
    for(int i = 0; Code[i]; i++){
        TokenSize++;
        token Token = Tokenize(Code + TokenOffset, TokenSize);
        if(Token.Enum != TOKEN_TYPE_INVALID){
            TokenOffset += TokenSize; 
            TokenSize = 0;
            
            ArrayAppend(Tokens, Token);
        }
    }

    For(Token, Tokens){
        //TokenPrint(Token);
    }

    ast Ast = Parse(Tokens, 0);
    
    ArrayFree(Tokens);
}

void LoadAndReadFile(const char * Path){
    static char NameBuff[128];
    ZeroMemory(NameBuff);
    sprintf(NameBuff, "../tests/stage_1/invalid/%s", Path);
    
    file File = FileOpen(NameBuff, "rb");

    char *Text = malloc(File.Size + 1);
    fread(Text, File.Size, 1, File.Handle);

    Text[File.Size] = 0;

    Compile(Text);
    
    free(Text);
    FileClose(File);
}

int main() 
{
    FileGLOB("../tests/stage_1/invalid/*.c", LoadAndReadFile);
	return 0;
}
