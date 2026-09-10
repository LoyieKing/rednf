int ss_sprintf(char* buffer, unsigned int count, const char* format, ...)
{
    // ORIG 0x08ad4548：va_list 槽（lea 0x14(%ebp) 单条）+ 返回值具名局部槽。
    va_list args;
    va_start(args, format);
    int r = ss_vsprintf(buffer, count, format, reinterpret_cast<char*>(args));
    va_end(args);
    return r;
