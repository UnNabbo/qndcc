b32 StringCompare(const char * String1, const char* String2){
    return(!strcmp(String1, String2));
}


b32 IsNumber(char Char){
    return ( Char >= '0' && Char <= '9');
}

b32 IsAlphabetical(char Char){
    return ( Char >= 'a' && Char <= 'z') || (Char >= 'A' && Char <= 'Z');
}

#if defined(__clang__)
#define PipeOpen popen	
#define PipeClose pclose
#elif defined(__GNUC__) || defined(__GNUG__)
#define PipeOpen popen	
#define PipeClose pclose
#elif defined(_MSC_VER)
#define PipeOpen _popen	
#define PipeClose _pclose
#endif

static s32 ExecuteCommand(char * Command){
	FILE* Console = PipeOpen(Command, "r");
	if (!Console) {
        printf("Failed to open pipe\n");
    }
	char PipeBuffer[1024];
	ZeroMemory(PipeBuffer);
	
	s32 ErrorCode = 0;

	while (fgets(PipeBuffer, sizeof(PipeBuffer), Console) != 0) {
		if (strstr(PipeBuffer, "error") || strstr(PipeBuffer, "fatal")) {
			ErrorCode = 1;
		}
		if (strstr(PipeBuffer, "Error") || strstr(PipeBuffer, "Fatal")) {
			ErrorCode = 1;
		}
	}

	PipeClose(Console);
	return ErrorCode;
}



static void FolderCreate(const char * Path){
    s8 Buffer[128];
    ZeroMemory(Buffer);
	sprintf(Buffer, ">nul 2>&1 pushd \"%s\" &&(popd & >nul 2>&1 mkdir \"%s\")|| >nul 2>&1 mkdir \"%s\"", Path,  Path,  Path);
	ExecuteCommand(Buffer);
}
