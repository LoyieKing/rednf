void CUser::SetChildrensDayEventValue(int slot, int targetItem, int period)
{
    if (slot > 9)
        return;
    // [b28d-27] ORIG 0x083281be：`1 << slot`（**int** 移位，非 1u）令 4.4 走
    // eax 累加器形（movzbl→eax; mov %eax,%edx; …; or %edx,%eax; mov %eax,%edx），
    // 与 ORIG 逐条全等（探针 /tmp/d27_p2.cpp f7；1u 形折单段 edx 见 f1）。
    m_childrensDayEventState =
        (unsigned char)(m_childrensDayEventState | 1 << slot);  // P0：无 state 槽
    m_childrensDaySlots[slot] = targetItem;
    m_childrensDayPeriods[slot] = period;
