void DNFFLib::fPrintTextFile(char* file, char* fmt, ...) {
    static const short MAX_INFO_LINE = 0x1000;  // ORIG _ZZN7DNFFLib14fPrintTextFileEPcS0_zE13MAX_INFO_LINE
    char buffer[0x1000];
    memset(buffer, 0, MAX_INFO_LINE);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);
    PrintTextFile(file, buffer);
