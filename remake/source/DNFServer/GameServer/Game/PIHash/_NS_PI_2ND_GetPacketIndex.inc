short _NS_PI_2ND_GetPacketIndex(char* packet)
{
    return *reinterpret_cast<short*>(packet + 1);
