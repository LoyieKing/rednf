int getRevisionToEnumValue(const std::string& s)
{
    if (s == "[reduce probability at pvp type module]")
        return 1;
    if (s == "[reduce probability at pvp module]")
        return 2;
    if (s == "[reduce probability to human armor]")
        return 0;
    if (s == "[reduce probability to human armor at pvp type module]")
        return 3;
    if (s == "[reduce probability to human armor at pvp module]")
        return 4;
    return 5;
