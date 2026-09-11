// ORIG 0x8541bb0：map 未命中或 state!=2 → 0x13；state==2 → 0。
int CItemLock::IsAbleItemUnlockCancel(unsigned char lock) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    // P133 独立守卫：ORIG `test;je <then>; mov$0x13;jmp end; then: cmp $2;setne;
    // test;je <0x13 尾>` ⇒ 两条件各自独立 if（合并短路会产 bool 汇聚异形）。
    if (it == m_lockMap.end())
        return 0x13;
    if (it->second.m_state != 2)
        return 0x13;
    return 0;
}