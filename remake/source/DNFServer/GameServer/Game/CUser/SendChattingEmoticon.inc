void CUser::SendChattingEmoticon()
{
    // ORIG 0x8689b90：GetGameOptionRef 结果**落槽 -0x10** 再作 this 调
    // getEmoticonInfo（sret 就地构 cfg@-0x22c）；行地址**每使用点重算**
    // （`imul $0x16` 段，无 row 缓存变量——strlen/put_str 各自 lea+imul）。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x19b);
    ((InterfacePacketBuf*)&packet)->put_short(0x18);
    CGameOption* go = GetGameOptionRef();
    CChattingEmoticonConfig cfg = go->getEmoticonInfo();
    for (int i = 0; i < 0x18; ++i)
    {
        ((InterfacePacketBuf*)&packet)->put_short(i);
        ((InterfacePacketBuf*)&packet)->put_short(cfg.m_Row[i].m_nCount);
        ((InterfacePacketBuf*)&packet)->put_int((int)strlen(cfg.m_Row[i].m_data));
        ((InterfacePacketBuf*)&packet)->put_str(cfg.m_Row[i].m_data,
                                                (int)strlen(cfg.m_Row[i].m_data));
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
