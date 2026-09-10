void CToolFileLog::operator()(char const* filename, int no, char const* format, ...) {
    char buff[0x7000] = {0};
    char argsbuff[0x7000] = {0};
    char fbuff[256] = {0};
    sprintf(buff, "%s(%d): ", name, level);
    va_list args;
    va_start(args, format);
    vsprintf(argsbuff, format, args);
    // 原始：strlen 内联于 strcat 参数
    strcat(buff + strlen(buff), argsbuff);
    // 原始：no != -1 分支在前（fall-through 写 subfile），no==-1 用 je 跳到后块
    // → if (no != -1) { sprintf+write(subfile) } else { write(file) }
    if (no != -1) {
        sprintf(fbuff, "%s_%02d_", filename, no);
        CFileLogWriterInstance()->writeLog(fbuff, buff);
    } else {
        CFileLogWriterInstance()->writeLog(filename, buff);
    }
