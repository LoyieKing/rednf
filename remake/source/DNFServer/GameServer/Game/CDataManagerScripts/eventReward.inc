// [b28d-solo-CDataManagerScripts] AE：map C1 + CSlotBoundChecker C1（+0x18 真实内嵌成员）+ clear；fresh check_tu_routed clean（39=39）；ORIG 0x083738a2。
eventReward::eventReward() : m_map0()
{
    // ORIG C1 0x83738a2：构造 map + CSlotBoundChecker 后 clear map。
    // CSlotBoundChecker 重建为静态数组实现（无 per-instance 成员），+0x18 保持 pad。
    m_map0.clear();
}