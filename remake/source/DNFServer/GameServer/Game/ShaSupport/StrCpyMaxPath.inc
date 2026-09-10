void StrCpyMaxPath(char* dest, const char* src)
{
    std::memset(dest, 0, 0x104);
    std::strncpy(dest, src, 0x103);
