int getSkillDataExpansionToEnumValue(const std::string& s)
{
    if (s == "[static]")             return 0;
    if (s == "[level]")              return 1;
    if (s == "[cooltime]")           return 2;
    if (s == "[mp]")                 return 3;
    if (s == "[maintain mp]")        return 4;
    if (s == "[skill consume item]") return 5;
    if (s == "[casting time]")       return 6;
    return 7;
