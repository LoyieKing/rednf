int getWorldType(const char* name)
{
    std::string tmp(name);
    if (tmp == "[common]")
        return 0;
    if (tmp == "[fair]")
        return 1;
    return 2;
