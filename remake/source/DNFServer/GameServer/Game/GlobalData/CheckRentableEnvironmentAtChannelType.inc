// ---- CheckRentableEnvironmentAtChannelType —— T 0x08271032（同名报告）------
bool CheckRentableEnvironmentAtChannelType(CUser& /*user*/)
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return false;
    }
    return true;
}