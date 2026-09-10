void CParty::changeBattleDataOnMoveMap(bool firstVisit)
{
    // [b28d-51] P183 禁引用缓存 + P246 u64 别名：ORIG 0x085a8ef4/0x085a8f1c 为
    // 零扩展 64 位整体存（mov $0,%edx; mov %eax,0x360; mov %edx,0x364），
    // 且无具名 now 局部（call 结果 eax 直存）；两次独立取时。
    bd(this).m_moveTick = GlobalData::s_systemTime_.getCurTickCount();
    bd(this).m_enterTick = GlobalData::s_systemTime_.getCurTickCount();

    if (!firstVisit)                                     // movzbl xor $1 test je
        ++bd(this).m_field68c;                           // +0x9b8
