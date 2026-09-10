bool DisPatcher_ResPvpRank::ExtractPacket(MSG_RES_PVP_RANK& msg, PacketBuf& packet)
{
    // ORIG 0x081f039e T（报告 DisPatcher_ResPvpRank/ExtractPacket.md 全文转写）：
    // PvpResultType::Clear → 11×get_short（短读后符号扩展写回 int 字段）→ 10×get_int。
    msg.m_result.Clear();
    // 字段名按 PvpResultType.h 权威版（HdrFieldG ODR 收敛后；视图偏移 = PvpResultType 内偏移）。
    short s[11];
    packet.get_short(s[0]);
    packet.get_short(s[1]);
    packet.get_short(s[2]);
    packet.get_short(s[3]);
    packet.get_short(s[4]);
    packet.get_short(s[5]);
    packet.get_short(s[6]);
    packet.get_short(s[7]);
    packet.get_short(s[8]);
    packet.get_short(s[9]);
    packet.get_short(s[10]);             // ORIG 无失败分支（恒返 1）
    packet.get_int(msg.m_result.m_avg[11]);     // +0x44
    packet.get_int(msg.m_result.m_field6c);     // +0x6c
    packet.get_int(msg.m_result.m_field70);     // +0x70
    packet.get_int(msg.m_result.m_field74);     // +0x74
    packet.get_int(msg.m_result.m_field78);     // +0x78
    packet.get_int(msg.m_result.m_avg[12]);     // +0x48
    packet.get_int(msg.m_result.m_count);       // +0x4c
    packet.get_int(msg.m_result.m_winPoint);    // +0x50
    packet.get_int(msg.m_result.m_field7c);     // +0x7c
    packet.get_int(msg.m_result.m_field80);     // +0x80
    msg.m_result.m_avg[0] = s[0];    // +0x18
    msg.m_result.m_avg[1] = s[1];    // +0x1c
    msg.m_result.m_avg[2] = s[2];    // +0x20
    msg.m_result.m_avg[3] = s[3];    // +0x24
    msg.m_result.m_avg[8] = s[4];    // +0x38
    msg.m_result.m_avg[9] = s[5];    // +0x3c
    msg.m_result.m_avg[6] = s[6];    // +0x30
    msg.m_result.m_avg[7] = s[7];    // +0x34
    msg.m_result.m_avg[4] = s[8];    // +0x28
    msg.m_result.m_avg[5] = s[9];    // +0x2c
    msg.m_result.m_avg[10] = s[10];  // +0x40
    return true;
