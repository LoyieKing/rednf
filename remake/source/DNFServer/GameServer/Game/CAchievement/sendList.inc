// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %edx↔%eax 互换（121 条逐条
// 长度/助记符全等仅寄存器名差），临时/引用/限定名不可翻。
void CAchievement::sendList()
{
    stAchievement_DBData data;
    getData((char*)&data);

    PacketGuard packet;
    packet.put_header(0, 0x166);
    packet.put_int(data.m_count);
#if !remake_verify
    cMyTrace(__PRETTY_FUNCTION__, 0, 5)("[DNF-DIAG ACH] count=%d", data.m_count);
#endif
    for (int i = 0; i < data.m_count; ++i)
    {
        packet.put_int((int)data.m_entries[i].m_id);
        packet.put_short((int)data.m_entries[i].m_trigger0);
        packet.put_short((int)data.m_entries[i].m_trigger1);
        packet.put_short((int)data.m_entries[i].m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
}