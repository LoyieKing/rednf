int stStackableBooster_t::tag2Type(const char* tag)
{
    if (strcmp(tag, "[avatar]") == 0) return 0;
    if (strcmp(tag, "[special avatar]") == 0) return 1;
    if (strcmp(tag, "[cera]") == 0) return 2;
    if (strcmp(tag, "[creature]") == 0) return 3;
    if (strcmp(tag, "[equipment]") == 0) return 4;
    if (strcmp(tag, "[stackable]") == 0) return 5;
    if (strcmp(tag, "[etc]") == 0) return 6;
    if (strcmp(tag, "[special creature]") == 0) return 7;
    if (strcmp(tag, "[emblem]") == 0) return 8;
    return 9;
