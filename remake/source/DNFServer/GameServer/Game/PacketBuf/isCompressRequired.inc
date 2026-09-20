bool PacketBuf::isCompressRequired(char* packet) const
{
    char* q = packet;
    if (*(unsigned short*)q == 2 || *(unsigned short*)q == 9 ||
        *(unsigned short*)q == 0x57)
    {
        return true;
    }
    return false;
}
