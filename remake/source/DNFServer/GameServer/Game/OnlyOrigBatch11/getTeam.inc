// ORIG T 0x088c2d7c："character"→0、"neutral"→0xc8、"monster"→0x64、
// "all"→2、默认 0x64。
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
}