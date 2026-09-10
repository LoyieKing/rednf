bool IS_PVP_CHANNEL(int channelType)
{
    return (channelType == 4) || (channelType == 6) ||
           (channelType == 5) || (channelType == 7) ||
           (channelType == 8) || (channelType == 9) ||
           (channelType == 10) || (channelType == 13) ||
           (channelType == 14) || (channelType == 15) ||
           (channelType == 16);
