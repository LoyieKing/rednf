// ---- cal_end_time —— ORIG 0x0827d024 ----
// 计算 premium 开始/结束时间（param_6 时按既有 premium 结束续期）。
void premium_helper_function::cal_end_time(CUser* user, long& start, long& end,
                                           long base, int type, bool bActive,
                                           int term)
{
    if (bActive)
    {
        // ORIG 0x827d03c：外层 GetPremiumInfo 返回经出参槽直传，仅内层结果
        // 具名槽 -0xc（P52/P0）。
        // b28c-L20: AE — 旧 [N 封顶] 两条判词均作废：① g_SPremiumInfo[type]
        // 访问现为单条 `mov <A>(%eax)`（全局 disp 折叠，与 ORIG 同形）；
        // ② 尾比较改书写序 limit 在左（见下）后 ORIG `cmp %eax,%edx; jle`
        // 逐条复现，非 N22。
        WongWork::SUserPremiumInfo* info =
            user->GetPremiumInfo()->GetPremiumInfo(type);
        if (info->m_type == type)
        {
            end = (long)info->m_end + (long)term * 0x15180;
            start = (long)info->m_start;
        }
        else
        {
            end = (long)term * 0x15180 + base;
        }
    }
    else
    {
        end = (long)term * 0x15180 + base;
    }
    // b28c-L20: AE — 尾比较 ORIG 0x827d0bf 为「RHS(limit) 先物化 edx、start 后载 eax、
    // `cmp %eax,%edx; jle`」= 源码书写序 limit 在左（探针 /tmp/l20_probe.cpp f2b：
    // `lim > start` 产 `cmp start,%eax; jle` 同族）；写 `start < limit` 4.4 恒
    // LHS 先载 + `jge`（旧 N22 判词作废）。
    if (g_SPremiumInfo[type].m_timeLimit != 0 &&
        g_SPremiumInfo[type].m_timeLimit > start)
    {
        end = (long)g_SPremiumInfo[type].m_timeLimit;
    }
}

