int _NS_PI_2ND_GetHackType(unsigned int type, unsigned char b, unsigned short c)
{
    unsigned int e =
        static_cast<unsigned int>(_NS_PI_2ND_GetHackTypeEnum(b, static_cast<short>(c)));
    unsigned int v =
        2 * (e + 0x3e8u) +
        type;
    if (v > 0xe0e)
        return 0xe0f;
    return v;
