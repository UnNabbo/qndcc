
typedef struct{
    FILE * Handle;
    u64 Size;
} file;

file FileOpen(const char* Path, const char* Mode){
    file File;
    File.Handle = fopen(Path, Mode);
    Assert(File.Handle, "File does not exist");
    
    fseek(File.Handle, 0, SEEK_END);
    File.Size = ftell(File.Handle);
    fseek(File.Handle, 0, SEEK_SET);  /* same as rewind(f); */

    return File;
}

s32 FileWriteFormatted(file File, char * Format, ...){
    va_list arg;
#ifndef _MSC_VER
	va_start (arg, Format);
#else
	va_start (arg, &Format);
#endif
    
    s8 Line[256];
    s32 Len = vsprintf(Line, Format, arg);
    fwrite(Line, Len, 1, File.Handle);
    
	va_end (arg);
    return Len;
}

void FileClose(file File){
    fclose(File.Handle);
}

char * PathExtractName(char * Path){
    s32 PathLength = strlen(Path);
    s32 Length = 0;
    
    for(int i = PathLength - 1; Path[i] != '/' && Path[i] != '\\'; i--){
        Length++;
    }
    char * Name = Path + PathLength - Length;

    for(;Name[Length - 1] != '.'; Length--){}
    char *AllocatedName = MemAlloc(Length);
    MemCopy(AllocatedName, Name, Length - 1);
    return AllocatedName;
}

char * PathExcludeName(char * Path){
    s32 PathLength = strlen(Path);
    for(; Path[PathLength - 1] != '/' && Path[PathLength - 1] != '\\'; PathLength--){}
    char *AllocatedName = MemAlloc(PathLength + 1);
    MemCopy(AllocatedName, Path, PathLength);
    return AllocatedName;
}

char * FileOpenAndRead(const char * Path){
    file File = FileOpen(Path, "rb");

    char *Text = malloc(File.Size + 1);
    fread(Text, File.Size, 1, File.Handle);

    Text[File.Size] = 0;

    FileClose(File);
    
    return Text;
}



static void FileGLOB(const char* Pattern, void (*Func)(const char * )){
	WIN32_FIND_DATA fd; 
	HANDLE hFind = FindFirstFile(Pattern, &fd);
	if(hFind != INVALID_HANDLE_VALUE) { 
		do { 
			if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
			    Func(fd.cFileName);
			}
		}while(FindNextFile(hFind, &fd)); 
		FindClose(hFind); 
	}
}
