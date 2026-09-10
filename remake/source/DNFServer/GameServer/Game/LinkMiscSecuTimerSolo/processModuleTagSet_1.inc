bool processModuleTagSet(std::string* tag, int& out)
{
    static const std::string moduleNameTag[5][2] = {
        {"[expert job none]", "[/expert job none]"},
        {"[magic enchant]",   "[/magic enchant]"},
        {"[alchemist]",       "[/alchemist]"},
        {"[disjointer]",      "[/disjointer]"},
        {"[doll controller]", "[/doll controller]"}
    };
    for (int i = 1; i <= 4; ++i)
    {
        if (*tag == moduleNameTag[i][0])
        {
            out = i;
            return 0;
        }
        if (*tag == moduleNameTag[i][1])
            return 0;
    }
    return 1;
