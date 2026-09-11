int WarRoom::SetSlotState(CUser* user, int seat, ENUM_SEAT_STATE state, CUser** out,
                          bool& a, bool& b, bool& c)
{
    // [b28d-solo-WarRoom] AE | ORIG 0x086bd0f8 | P235 union 具型视角
    //   （m_slotsUser）消 `(CUser*)m_slots[]` cast 的二段装载后逐条全等。
    if (user == 0)
        return 0;
    // [b28BM13 P209] ORIG 0x086bd0f8 形 = `mov 0x14(%ebp),%eax; cmp $0xff,%eax`
    // （无符号比较先载寄存器）；enum 直比 4.4 折 cmpl mem。
    // [b28BM13 P222] 正向包裹：ORIG 不等臂 jne 直达共享尾 return-0 块。
    if (static_cast<unsigned int>(state) == 0xffu)
    {
        if (m_slots[seat] == 0)
        {
            // [b28BM13] ORIG 0x086bd12e：trace 块尾独立物化 `mov $0,%eax; jmp
            // 共享 epilogue`（显式 return 触发，块内无 store）。
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x590, 5);
            trace("WARAREA  WarRoom::SetSlotState players_[pos] is NULL pos(%d)", seat);
            return 0;
        }
        else
        {
            // [b28BM13] ORIG 0x086bd17e 单段直落 %edx；4.4 恒 eax 落值+mov
            // 保活两段（N18a；void** 无 cast 探针单段但 mangled 冻结 CUser**，
            // 语句表达式/memcpy 视图均不可翻，探针 bm13_p3/p5）。
            *out = m_slotsUser[seat];
            m_slotState[seat] = 0xff;
            // [b28BM13] ORIG 0x086bd1a1 `movl $0x0,0xc(%eax,%edx,4)` 实证清零。
            m_slots[seat] = 0;
            m_liveFlag[seat] = 0;
            a = true;
            if (GetState() > 2)
            {
                AddBanList(user);
            }
            user->getHackAnalyzer()->reportHackInfo();
            user->getHackAnalyzer()->resetHackInfo();
            if (m_pDungeon != 0)
            {
                user->m_historyLog.LeaveDungeon(m_pDungeon->GetDungeonName(),
                                                (unsigned char)m_levelBand);
            }
        }
    }
    return 0;
}