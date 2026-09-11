// ---- ORIG T 0x08990f66：纹章槽前缀→位标志（strncmp 前缀匹配；[All socket]
//      长度 12；其余 10；失配→0） ----
int getAvatarEmblemType(const char* name)
{
    if (strncmp(name, "[A socket]", 10) == 0)
        return 1;
    if (strncmp(name, "[B socket]", 10) == 0)
        return 2;
    if (strncmp(name, "[C socket]", 10) == 0)
        return 4;
    if (strncmp(name, "[D socket]", 10) == 0)
        return 8;
    if (strncmp(name, "[S socket]", 10) == 0)
        return 0x10;
    if (strncmp(name, "[M socket]", 10) == 0)
        return 0xffef;    // ORIG mov $0xffef (65519)
    if (strncmp(name, "[All socket]", 12) == 0)
        return 0xffff;     // ORIG mov $0xffff (65535)
    return 0;
}