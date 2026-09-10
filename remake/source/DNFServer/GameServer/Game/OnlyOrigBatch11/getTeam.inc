int getTeam(const char* name)
{
    if (strcasecmp(name, "character") == 0)
        return 0;
    if (strcasecmp(name, "neutral") == 0)
        return 0xc8;
    if (strcasecmp(name, "monster") == 0)
        return 0x64;
    if (strcasecmp(name, "all") == 0)
        return 2;
    return 0x64;
