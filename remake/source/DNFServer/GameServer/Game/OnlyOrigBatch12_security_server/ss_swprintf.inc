int ss_swprintf(wchar_t* buffer, unsigned int count, const wchar_t* format, ...)
{
    // ORIG 0x08ad45bf：va_list 槽（lea 0x14(%ebp)）+ 返回值具名局部槽。
    va_list args;
    va_start(args, format);
    int r = ss_vswprintf(buffer, count, format, (char*)args);
    va_end(args);
    return r;
