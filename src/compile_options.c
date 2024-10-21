typedef struct{
    char * AsmPath;
} compile_options;

s32 CompileOptionsSelect(compile_options* Options, char * Literal){

    return 0;
}

char ** FilesAndOptionsFind(compile_options * Options,char * Argv[], s32 Argc){
    char ** Files = ArrayAlloc(char **);
    for(int i = 1; i < Argc; i++){
        char* Literal = Argv[i];
        if(Literal[0] == '-' || Literal[0] == '/'){
            if(StringCompare(Literal + 1, "Fo")){
                    Options->AsmPath = Argv[++i];
            }
        }else{
            ArrayAppend(Files, Literal);
        }
    }
    return Files;
}
