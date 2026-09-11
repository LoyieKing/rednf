// ORIG T 0x088dd1b2：strcmp 线性查表，命中返回下标，未命中返回 -1。
int getChannelType(const char* name)
{
    for (int i = 0; i <= 0x13; ++i)
    {
        if (strcmp(name, g_ChannelTypeString[i]) == 0)
            return i;
    }
    return -1;
}
