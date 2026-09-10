int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList)
{
    // ORIG _Z11ss_vsprintfPcjPKcS_ @0x08ad457c
    int ret = vsnprintf(buffer, count, format, argList);
    if (static_cast<int>(count) <= ret)
    {
        *buffer = '\0';
        return 0;
    }
    return ret;
