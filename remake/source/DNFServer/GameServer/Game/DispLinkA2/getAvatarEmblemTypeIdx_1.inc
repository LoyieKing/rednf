int getAvatarEmblemTypeIdx(unsigned short type)
{
    if (type > 0x10)
        return -1;
    for (int i = 4; i >= 0; --i)
    {
        if ((type >> i) == 1)
            return i;
    }
    return -1;
