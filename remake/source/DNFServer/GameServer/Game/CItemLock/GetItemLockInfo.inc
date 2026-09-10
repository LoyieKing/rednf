int CItemLock::GetItemLockInfo(unsigned char lock, int& outSec) const
{
    // [b28c-6] P222 已修（5→1）：残差为返回归一化，已由 int 返回类型消解。
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(lock);
    // P222 正向包裹：ORIG `test;je 尾部零块; <then>; …; jmp end; 零块:mov $0`
    // ⇒ true 侧包块、false 归共享尾（否定早返 `if(it==end)return 0;` 产反向岛）。
    if (it != m_lockMap.end())
    {
        outSec = it->second.m_unlockSec;
        return it->second.m_state;
    }
    return 0;
