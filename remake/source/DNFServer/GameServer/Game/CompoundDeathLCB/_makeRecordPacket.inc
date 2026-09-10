void CDeathTowerRanking::_makeRecordPacket(const stBestRecord_t& record,
                                           PacketGuard& packet)
{
    for (int i = 0; i < 4; ++i)
    {
        // ORIG 0x08468cd6/0x08468d03：strlen **两次独立调用**（每次完整重发
        // record+i*0x1e+4 两级寻址 + call），put_str 用第二次结果（P48/P183）。
        packet.put_int(
            static_cast<int>(strlen(record.m_memberData[i])));
        // ORIG 0x8468d37 调 const 重载 put_str(PKci)@0x0822b770（非 Pci 版）。
        packet.put_str(
            record.m_memberData[i],
            static_cast<int>(strlen(record.m_memberData[i])));
        packet.put_byte(record.m_pad7c[1][i]);                // +0x80..+0x83
        packet.put_byte(record.m_pad7c[2][i]);                // +0x84..+0x87
    }
    packet.put_short(static_cast<int>(record.m_stage));        // +0x88
    packet.put_int(static_cast<int>(record.m_playTime));       // +0x8c
    packet.put_int(record.m_field94);                          // +0x94
