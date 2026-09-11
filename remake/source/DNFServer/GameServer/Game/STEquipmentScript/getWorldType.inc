// ---- ORIG T 0x088c1492：world 标签→类型 ----
int getWorldType(const char* name)
{
    std::string tmp(name);
    if (tmp == "[common]")
        return 0;
    if (tmp == "[fair]")
        return 1;
    return 2;
}