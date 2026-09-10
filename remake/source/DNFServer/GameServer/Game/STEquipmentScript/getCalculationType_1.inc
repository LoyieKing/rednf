int getCalculationType(const char* name)
{
    std::string tmp(name);
    if (tmp == "+")
        return 0;
    if (tmp == "%")
        return 1;
    return 2;
