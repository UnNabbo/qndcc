b32 StringCompare(const char * String1, const char* String2){
    return(!strcmp(String1, String2));
}


b32 IsNumber(char Char){
    return ( Char >= '0' && Char <= '9');
}

b32 IsAlphabetical(char Char){
    return ( Char >= 'a' && Char <= 'z') || (Char >= 'A' && Char <= 'Z');
}

