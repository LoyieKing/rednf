int getActiveStatusType(const std::string& s)
{
    static const char* kTag[] = {
        "slow", "freeze", "poison", "stun", "curse", "blind", "lightning",
        "stone", "sleep", "burn", "weapon break", "bleeding", "haste",
        "bless", "element", "confuse", "hold", "armor break",
    };
    static const char* kNoBox[] = {
        "[slow]", "[freeze]", "[poison]", "[stun]", "[curse]", "[blind]",
        "[lightning]", "[stone]", "[sleep]", "[burn]", "[weapon break]",
        "[bleeding]", "[haste]", "[bless]", "[element]", "[confuse]",
        "[hold]", "[armor break]",
    };
    for (int i = 0; i <= 0x11; ++i)
    {
        if (s == kTag[i] || s == kNoBox[i])
            return i;
    }
    return 0x12;
