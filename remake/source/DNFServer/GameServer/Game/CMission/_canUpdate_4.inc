bool CMission_kill::_canUpdate(const MissionClearCondition_Parameter& param) const
{
    // ORIG W 0x085e3bc4：cmp m_minKill, m_killCount; jl <false-block> → mov $1（killCount>=minKill 返回 true）。
    // 修正：先前 source 写了 killCount < minKill（方向反了），按 ORIG 改为 >=。
    if (param.m_killCount >= m_minKill)
        return true;
    return false;
}