// ============================================================================
// CVillageMonster::OnKillVillageMonster(CUser*,int,int,int,bool)
//   —— ORIG 0x86b34a0
//   objdump: if (m_state != 1) return 0x13;
//            if (user && user->GetPartyIndex() != m_partyIndex) return 0x13;
//            if (flag) { OnChangeState(2); TimerQueue::InsertTimer(2,0,0x86,c,a,b);
//                        SendVillageMonsterFightResult(user, true); }
//            else     { OnChangeState(0); SendVillageMonsterFightResult(user, false); }
//            return 0;
// ============================================================================
// [round2-L30 修] 否定极性 + 分支交换：ORIG 0x086b34ef `movzbl -0xc(%ebp),%eax;
//   xor $0x1,%eax; test %al,%al; je <+0x8e>`（!flag 为真则落 fall-through 的
//   OnChangeState(0)/SendFight(false) 段，+0x8e 为 true 段）⇒ 源形必写
//   `if (!flag) { …0… } else { …2… }`（P222/P214①；正极性 `if (flag)` 4.4 产直判
//   `cmpb; je` 且块序相反）。
int CVillageMonster::OnKillVillageMonster(CUser* user, int a, int b, int c, bool flag)
{
    if (m_state != 1)
        return 0x13;
    if (user != 0)
    {
        if (user->GetPartyIndex() != m_partyIndex)
            return 0x13;
    }
    if (!flag)
    {
        OnChangeState(ENUM_VILLAGE_MONSTER_STATE_0);
        // ORIG 0x086b350d/0x086b357b：两分支都 `movzbl -0xc(%ebp),%eax;
        //   mov %eax,0x8(%esp)` ⇒ 实参是 flag 变量本身（非 false/true 常量）。
        SendVillageMonsterFightResult(user, flag);
        return 0;                       // ORIG 0x086b3527 `mov $0x0,%eax; jmp`（分支内独立设值）
    }
    else
    {
        OnChangeState(ENUM_VILLAGE_MONSTER_STATE_2);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x86, c, a, b);
        SendVillageMonsterFightResult(user, flag);
        return 0;                       // ORIG 0x086b3595 `mov $0x0,%eax`
    }
}