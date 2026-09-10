int Inter_GuildCargoHistory::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoHistoryPktView* pPkt = reinterpret_cast<Inter_GuildCargoHistoryPktView*>(data);
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
        return 0;

    int putCount = 0;                                // local_14/asm -0x10(%ebp)
    PacketGuard packet;
    packet.put_header(1, 0x10a);
    packet.put_byte(1);
    // （asm("ebx") 钉形被 put_int(int&,int) 取址阻断，保持具名槽。）
    int countIdx = packet.get_index();         // 计数回填位
    packet.put_int(putCount);
    for (int i = 0; i <= 0x31; ++i)                  // asm cmpl $0x31, setle
    {
        if (*reinterpret_cast<int*>(
                data + (i * 3 + 1) * 0x10 + 2) != 0)
        {
            packet.put_binary(
                data + (i * 3 + 1) * 0x10 + 2, 0x30);
            ++putCount;
        }
    }
    packet.put_int(countIdx, putCount);              // put_int(int&,int) 回填
    packet.finalize(true);
    user->Send(packet);
    user->m_historyLog.GuildCargoHistoryResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)4, putCount);
    return 0;
