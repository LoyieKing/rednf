int getChannelType(const char* name)
{
    for (int i = 0; i <= 0x13; ++i)
    {
        if (strcmp(name, g_ChannelTypeString[i]) == 0)
            return i;
    }
    return -1;
