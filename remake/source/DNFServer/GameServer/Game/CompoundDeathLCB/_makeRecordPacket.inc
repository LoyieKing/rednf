// ORIG T 0x08468cb6。每名成员：strlen 名字(int)+名字串、两个槽位 byte
// （stBestRecord_t +0x80/+0x84 即 m_pad7c[4+i]/m_pad7c[8+i]）、层号 short、
// 通关耗时 int、名次 int。
// 【batch-12 标注】o=89 n=88（gap 10 → 1）：strlen 两次独立调用已补齐（P48）；余 1
//   为 ORIG 尾 `call leave ret` 前的 mov + 对齐 nop（§6.1 豁免）⇒ N 封顶。
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
}