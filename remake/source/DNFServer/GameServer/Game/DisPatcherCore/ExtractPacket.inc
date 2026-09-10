bool DisPatcher_MakePVP::ExtractPacket(PacketBuf& packet, MSG_MAKE_PVP_ROOM& msg)
{
    // ORIG 0x081f81ba T（报告 DisPatcher_MakePVP/ExtractPacket.md 全文转写）。
    char tail;
    if (!packet.get_byte(msg.m_field0d) || msg.m_field0d < 0)
    {
        return false;
    }
    if (msg.m_field0d == 0)
    {
        int len = 0;
        if (!packet.get_int(len) || len < 0 || len >= 0x1e)
        {
            return false;
        }
        if (!packet.get_str(msg.m_name, 0x1e, len))
        {
            return false;
        }
    }
    if (!packet.get_short(msg.m_field2c))
    {
        return false;
    }
    if (!packet.get_byte(msg.m_field2e))
    {
        return false;
    }
    if (msg.m_field2e == 1)
    {
        if (!packet.get_int(msg.m_field2f))
        {
            return false;
        }
        if (msg.m_field2f > 8)
        {
            return false;
        }
        if (!packet.get_binary(msg.m_field33, msg.m_field2f))
        {
            return false;
        }
    }
    if (!packet.get_byte(tail))
    {
        return false;
    }
    // ORIG +0x1a9：先无条件 movb $0，再 cmp $1/jne 分支补写 1（非 setne
    // 物化）→ 两步赋值形态。
    msg.m_battleMode = 0;
    if (tail == 1)
    {
        msg.m_battleMode = 1;
    }
    return true;
