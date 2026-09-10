int MI_GetHackType(char c)
{
    int v = (c & 0xff) + 0x6a4;
    if (v > 0x707)
        v = 0x707;
    return v;
