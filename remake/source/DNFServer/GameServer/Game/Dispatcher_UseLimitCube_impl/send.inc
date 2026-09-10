void Dispatcher_UseLimitCube::send(CUser* user, ParamBase& param)
{
    stUseLimitCube& st = *reinterpret_cast<stUseLimitCube*>(&param);
    if (st.m_4 != 0)                     // +0x04（ORIG movzbl + test %al 直判）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x153, st.m_4);
        return;
    }
    int resultIdx = st.m_8;              // +0x08 movl → put_int
    int resultCount = st.m_c;            // +0x0c movl → put_short（截断 cwtl）
    PacketGuard packet;
    packet.put_header(1, 0x153);
    packet.put_byte(1);
    packet.put_int(resultIdx);
    packet.put_short(resultCount);
    packet.finalize(true);
    user->Send(packet);
