void CMyFileLog::operator()(char const* filename, char const* format, ...) {
    char buff[0x7000] = {0};
    sprintf(buff, "%s(%d): ", name, level);
    va_list args;
    va_start(args, format);
    // 原始：strlen 内联于 vsprintf 参数（不存局部变量）
    vsprintf(buff + strlen(buff), format, args);
    // 原始：Instance() 结果直接作为实参
    CFileLogWriterInstance()->writeLog(filename, buff);  // 原始：缓冲区直接作为格式串传递
