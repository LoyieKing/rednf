int getExcepttionStatusToEnumValue(std::string& s)
{
    if (s == "silence")
        return 1;
    if (s == "disease")
        return 3;
    if (s == "mindcontrol")
        return 2;
    if (s == "all")
        return 5;
    return -1;
