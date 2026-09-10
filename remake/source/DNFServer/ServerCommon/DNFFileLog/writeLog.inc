void CFLog::writeLog(tm* t, char const* fmt, char* va) {
    if (file == NULL) {
        return;
    }
    char message[0x7000];
    vsprintf(message, fmt, va);
    char line[0x7000];
    sprintf(line, "[%02d:%02d:%02d] %s", t->tm_hour, t->tm_min, t->tm_sec, message);
    fprintf(file, "%s\r\n", line);
    fflush(file);
