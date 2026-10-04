int WarRoom::ReviveUserByCoin(CUser* user, unsigned short uid)
{
    // [b28d-solo-WarRoom-3] ORIG 0x086bf324：97/98 → **98/98（gap 0）**，判据①
    //   jcc 目标下标逐位全等。修法 = P223 定案形：外层失败改负条件
    //   `if (m_slots[slot] == 0) goto REVIVE_NEXT;` + 循环体尾空标签
    //   （ORIG 086bf38a `je 086bf3da(nop 岛)` 与 086bf3aa `je 086bf3db(addl)`
    //   落点不同）⇒ 4.4 不再 jump-thread，nop 中继岛逐条复现。
    //   b25 已补 target==0 → 0x15 语义块。
    // ORIG 0x86bf33c：短路 && 条件直进 if（bool 局部会 spill cmpb）。
    if ((GetState() != 3) && (GetState() != 4))
        return 0x13;
    CUser* target = 0;
    int slot;
    for (slot = 0; slot <= 5; ++slot)
    {
        // [b28d-solo-WarRoom-3] P223 定案形：ORIG 086bf38a `je 086bf3da(nop 岛)`
        // 与 086bf3aa `je 086bf3db(addl)` 落点不同 ⇒ 外层失败走**负条件 goto
        // 到循环体尾空标签**（4.4 不再 jump-thread，逐条复现 nop 中继岛）。
        if (m_slots[slot] == 0)
            goto REVIVE_NEXT;
        if ((m_slotsUser[slot])->get_unique_id() == uid)
        {
            if (m_liveFlag[slot] != 0)
                return 0x12;
            target = m_slotsUser[slot];
            // ORIG 086bf3d8 jmp→086bf3ea（break 出口）。
            goto LOOP_END;
        }
REVIVE_NEXT:
        ;
    }
LOOP_END:
    ;
    // ORIG 0x86bf3fb：target 空守卫 return 0x15（原重建缺失该语义块）。
    if (target == 0)
        return 0x15;
    if (user->GetUsedCoinCount() >= m_pDungeon->m_87c)
        return 0x16;
    HistoryLog::WriteUseCoin((_IO_FILE*)user->m_historyLogFp,
                             target->getCurCharacName());
    SetCharacterLive(slot, target, true, true, 0, 0xff);
    return 0;
}