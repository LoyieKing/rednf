// ORIG T 0x089bce65（const string&）：
// "[reduce probability at pvp type module]"→1、
// "[reduce probability at pvp module]"→2、
// "[reduce probability to human armor]"→0、
// "[reduce probability to human armor at pvp type module]"→3、
// "[reduce probability to human armor at pvp module]"→4、默认 5。
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
}