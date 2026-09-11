char* PacketBuf::get_packet(int size)
{
    if (size > MAX_PACKET_SIZE - 1)
    {
        cMyTrace ct(__FUNCTION__, 0x170, 5);
        ct("[%s][%d]", __FUNCTION__, 0x170);
    }
    return m_ptr + size;
}