eventReward::eventReward() : m_map0()
{
    // ORIG C1 0x83738a2：构造 map + CSlotBoundChecker 后 clear map。
    // CSlotBoundChecker 重建为静态数组实现（无 per-instance 成员），+0x18 保持 pad。
    m_map0.clear();
