// ============================================================================
// CBattle_Field::setEplpWaitingTimer —— ORIG 0x0830ac7a（0x17b 字节）
//   报告：docs/class_func_reports/CBattle_Field/setEplpWaitingTimer.md
//   type 3/6 → 绝望之塔 timer(0x4d)；4/5 → party timer(0x25)；其余 → party
//   timer(0x15)。秒数均 0x1e。
//   （N 封顶：ndiff 93 残差为分支判定调度（cmp $4,jge vs $3,jle）+ 三段
//   InsertTimer 链 esi/ebx 跨调用保存与帧差异，源码已按 ORIG 比较边界写 >=4，
//   GCC 仍规范化，无法逐字节复现——见报告）
// ============================================================================
// [b28d-solo-CBattle_Field] ORIG 0x0830ac7a：帧 sub $0x30（push esi/ebx）已对齐；
// key 直落 %esi（无 ebx 中转）⇒ 去 static_cast（P41 变体）与形参 register（P248）。
void CBattle_Field::setEplpWaitingTimer(QuickParty::RandomBuffDungeon type)
{
    if (m_party == 0)
        return;

    // [b28d-solo-CBattle_Field] P46/P261：ORIG 0x0830ac92 决策链为
    // `cmp $5;jg / cmp $4;jge / cmp $3;je / jmp / cmp $6;jne`（无状态槽的
    // 寄存器 cmp 链）⇒ 原码是 switch(type)，if-else 链在 4.4 恒产
    // `cmp $3;jle` + 内存直比（探针 /tmp/s18/p4.cpp f3 逐条命中决策链）。
    // 注：RandomBuffDungeon 占位枚举仅 NONE=0，4.4 -fstrict-enums 会把
    // case 3..6 判为不可达并整体删除 ⇒ 以 (int) 视图 switch（不改共享头）。
    switch ((int)type)
    {
    case 3:
    case 6:
    {
        WongWork::CDeathTower* tower = m_party->getManager()->getDeathTower();
        if (tower == 0)
            return;
        register int key = tower->genTimerKey((TIMER_MESSAGE)0x4d);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)6, tower->getIdx(),
                                    (TIMER_MESSAGE)0x4d, 0x1e, key, 0x4d);
        return;
    }
    case 4:
    case 5:
    {
        // P0/P24：ORIG 0x0830ad31 段 idx 直落 %ebx（无具名槽）
        // ⇒ GetPartyIndex() 内联进 InsertTimer 实参。
        register int key = m_party->gen_timer_key((TIMER_MESSAGE)0x25);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    m_party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x25, 0x1e, key, 0x25);
        return;
    }
    default:
        break;
    }
    register int key = m_party->gen_timer_key((TIMER_MESSAGE)0x15);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, m_party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x15, 0x1e, key, 0x15);
}