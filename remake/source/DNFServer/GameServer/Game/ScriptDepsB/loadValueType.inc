int loadValueType(const std::string& token)
{
    if (token == "+")
        return 0;
    if (token == "%")
        return 1;
    return 2;
