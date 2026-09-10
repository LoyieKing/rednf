bool DisPatcher_ResPvpRank::ExtractPacket(MSG_RES_PVP_RANK& msg, PacketBuf& packet)
{
    // ORIG 0x081f039e T（报告 DisPatcher_ResPvpRank/ExtractPacket.md 全文转写）：
    // PvpResultType::Clear → 11×get_short（短读后符号扩展写回 int 字段）→ 10×get_int。
    msg.m_result.Clear();
    // [G5-DisPatcherCore] P0/P161：ORIG 在 Clear 之后 `mov 0xc(%ebp),%eax; add $0xd,%eax;
    // mov %eax,-0xc(%ebp)` = **具名基址槽 -0xc**；其后每处读取均为
    // `mov -0xc(%ebp),%eax; add $0x44/$0x6c/…(%eax)`（基址+偏移两步）。我方原先写
    // `msg.m_result.xxx` 被折叠成单条 `add $0x51(%eax)`（0xd+0x44）⇒ 全函数形态相异。
    PvpResultType& result = msg.m_result;
    // ORIG 的 11 个 short 是**独立具名局部**（槽 -0xe/-0x10/…/-0x20，逐条 `lea -0xN(%ebp)`），
    // 不是数组 —— 数组形（我方原 `short s[11]`）恒产 `lea 基址; add $2k`。
    short s0;
    short s1;
    short s2;
    short s3;
    short s4;
    short s5;
    short s6;
    short s7;
    short s8;
    short s9;
    short s10;
    packet.get_short(s0);
    packet.get_short(s1);
    packet.get_short(s2);
    packet.get_short(s3);
    packet.get_short(s4);
    packet.get_short(s5);
    packet.get_short(s6);
    packet.get_short(s7);
    packet.get_short(s8);
    packet.get_short(s9);
    packet.get_short(s10);             // ORIG 无失败分支（恒返 1）
    packet.get_int(result.m_avg[11]);     // +0x44
    packet.get_int(result.m_field6c);     // +0x6c
    packet.get_int(result.m_field70);     // +0x70
    packet.get_int(result.m_field74);     // +0x74
    packet.get_int(result.m_field78);     // +0x78
    packet.get_int(result.m_avg[12]);     // +0x48
    // [G5-DisPatcherCore] 真 bug 修复：ORIG 该两处 `add $0x4c/%0x50,%eax` = **m_avg[13]/m_avg[14]**。
    // m_count 的权威偏移是 +0x54（ORIG PvpResultType::AddNewResult 0x085d5944 段 0x54(%eax) 出现 18 次，
    // 与 PvpResultType.h 布局 m_avg[15]@0x18 一致）；原写 m_count/m_winPoint 会错写 +0x54/+0x58。
    packet.get_int(result.m_avg[13]);     // +0x4c
    packet.get_int(result.m_avg[14]);     // +0x50
    packet.get_int(result.m_field7c);     // +0x7c
    packet.get_int(result.m_field80);     // +0x80
    result.m_avg[0] = s0;    // +0x18
    result.m_avg[1] = s1;    // +0x1c
    result.m_avg[2] = s2;    // +0x20
    result.m_avg[3] = s3;    // +0x24
    result.m_avg[8] = s4;    // +0x38
    result.m_avg[9] = s5;    // +0x3c
    result.m_avg[6] = s6;    // +0x30
    result.m_avg[7] = s7;    // +0x34
    result.m_avg[4] = s8;    // +0x28
    result.m_avg[5] = s9;    // +0x2c
    result.m_avg[10] = s10;  // +0x40
    return true;
