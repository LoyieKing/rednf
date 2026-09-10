void CFLog::writeLog(char const* fmt, char* va) {
    if (file == NULL) {
        return;
    }
    char message[0x7000];
    vsprintf(message, fmt, va);
    fprintf(file, "%s\r\n", message);
    fflush(file);
