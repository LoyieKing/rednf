void CDataManager::get_mob_reward(int level, unsigned int& out) const
{
    if (level <= 0 || level > 199)
    {
        out = 0;
        return;
    }
    // [b28d-solo-CDataManager] ORIG `mov 0x4(%eax,%edx,4),%edx` 单段直落 edx
    // （unsigned 目标与源同型）；源 int 数组直读会多 `mov %eax,%edx` 中转。
    // 修法 = CDataManager.h 的匿名 union 同址 unsigned 视角 m_mobRewardU
    // （P235/P246，sizeof/偏移/mangled 零影响；探针 /tmp/p_sp/f.cpp 逐条全等）。
    out = m_mobRewardU[level];
