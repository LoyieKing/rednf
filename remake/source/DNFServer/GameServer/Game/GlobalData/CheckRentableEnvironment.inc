// ---- CheckRentableEnvironment —— T 0x08271002（同名报告）-------------------
bool CheckRentableEnvironment(CUser& /*user*/)
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return false;
    }
    return true;
}