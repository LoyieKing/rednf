int Inter_NoticeGuildCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeGuildCargoPktView* pPkt = reinterpret_cast<Inter_NoticeGuildCargoPktView*>(data);
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle; je 返回
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
        return 0;

    user->m_historyLog.GuildCargoResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)2, 0,
                           pPkt->m_18ea);
    user->SetGuildCargoCapacity(                     // msg+0x18ea 容量
        pPkt->m_18ea);

    int countIdx = 0;                                // local_1c/asm -0x18(%ebp)
    int putCount = 0;                                // local_14/asm -0x10(%ebp)
    PacketGuard packet;
    packet.put_header(0, 0xed);
    packet.put_int(pPkt->m_18ea);
    countIdx = packet.get_index();                   // 回填位
    packet.put_short(putCount);
    for (int i = 0; i <= 0x77; ++i)                  // asm cmpl $0x77, setle
    {
        // ORIG 循环 `mov -0x14(%ebp),%edx` 基址取自具名槽（非 0x10(%ebp) 形参槽）⇒
        // 元素访问经同一具名 base（pPkt）而非形参，避免第二指针槽（P89/P0）。
        // 残：ORIG `mov -0xc(%ebp),%eax; mov -0x14(%ebp),%edx; imul; lea`（先 i 后 base）
        // vs 4.4 恒 base-first（`i + ptr` 形实测同 base-first）⇒ P218/P30 装载序封顶。
        if (reinterpret_cast<const Inter_NoticeGuildCargoTail*>(
                (reinterpret_cast<const Inter_NoticeGuildCargoEntry*>(pPkt) + i)
                    ->m_body.m_bytes)->m_3 != 0)
        {
            packet.put_short((short)i);              // ORIG `cwtl`（源码 (short) 截断）
            packet.put_binary(
                (reinterpret_cast<const Inter_NoticeGuildCargoEntry*>(pPkt) + i)
                    ->m_body.m_bytes + 2, 0x35);
            ++putCount;
        }
    }
    packet.put_short(countIdx, putCount);            // put_short(int&,int) 回填
    packet.finalize(true);
    user->Send(packet);
    return 0;
