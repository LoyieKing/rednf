int CItemLock::IsAbleItemUnlock(unsigned char lock, int& outSec) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    // P133 独立守卫（同 IsAbleItemUnlockCancel）：ORIG `test;je <then>;
    // mov$0x13;jmp end; then: cmp $1;setne;test;je <0x13>` ⇒ 两条独立 if。
    if (it == m_lockMap.end())
        return 0x13;
    if (it->second.m_state != 1)
        return 0x13;
    // P171 照抄原码：ORIG 0x8541b2a 全 43 条内**没有** outSec 写入（唯一 ptEv
    // 只读 +0x8 m_state，无 +0x4 装载）⇒ 原码不写 outSec，保留形参未用。
    (void)outSec;
    return 0;
