void CMyRawFileLog::operator()(char const* filename, char const* format, ...) {
    char buff[0x7000] = {0};
    va_list args;
    va_start(args, format);
    vsprintf(buff, format, args);
    // 原始：Instance() 结果直接作为实参（不存局部变量）
    CFileLogWriterInstance()->writeRawLog(filename, buff);
}
