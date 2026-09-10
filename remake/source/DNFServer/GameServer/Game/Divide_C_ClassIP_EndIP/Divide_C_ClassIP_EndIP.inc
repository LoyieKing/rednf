void Divide_C_ClassIP_EndIP(char* out, char* end, char* src)
{
    if (src == 0)
        return;

    char* dot = std::strrchr(src, '.');  // +0x14: call _ZSt7strrchrPci
    if (dot == 0)
        return;

    unsigned int len = dot - src;                   // -0x10(%ebp)
    if (len > 12)                                   // cmpl $0xc,-0x10; ja
        return;

    unsigned int endLen = strlen(src + len + 1);    // -0xc(%ebp)
    if (endLen > 4)                                 // cmpl $0x4,-0xc; ja
        return;

    if (out != 0)
        strncpy(out, src, len);
    if (end != 0)
        strncpy(end, src + len + 1, endLen);
    return;
