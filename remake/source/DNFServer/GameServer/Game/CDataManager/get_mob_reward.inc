// 残余 DIFF 归因：ORIG else 臂把表值直读进 %edx 后 `mov %edx,(%eax)`，4.4.7
// 先落 %eax 再 spill %edx（N7/N18a 寄存器指派反向），界检查折叠同因。
// [b28d-solo-CDataManager AE] 修复：CDataManager.h 匿名 union 增同址 unsigned
// 视角 m_mobRewardU（P235/P246）复现 ORIG 单段 `mov 0x4(%eax,%edx,4),%edx`。
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
}