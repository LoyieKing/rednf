// ORIG 0x854261e：map 查找；命中返回 state 并写解锁时刻，未命中返回 0。
// [b28d-41] AE（compare_tu 口径，离开 DIFF）：返回类型 uchar→int 复现 ORIG 尾
// 双段零扩展（详见下行）。
// [b28d-41] 返回类型 uchar→int：ORIG 尾 `movzbl 0x8(%eax),%eax; movzbl %al,%eax`
// 双段零扩展由 int 返回 + uchar 字段表达式复现（探针 /tmp/d41_p4.cpp gB；P248 档①），
// 4.4 对 uchar 返回恒折叠单段。返回类型不入 mangled，调用点隐式截断语义不变。
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
}