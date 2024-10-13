
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

void FileClose(file File){
    fclose(File.Handle);
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
