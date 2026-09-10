int CParty::get_rate_level_difference()
{
    if (m_battleField.m_pDungeon == 0)
        return 0;
    // [b28d-51] ORIG 0x085b5a07 用 `sar $0x1f/xor/sub` 位技巧取绝对值（4.1 对
    // __builtin_abs 的形）；`if (diff<0) diff=-diff;` 在 4.4 恒产 negl 分支形。
    const int diff = __builtin_abs(
        m_battleField.m_pDungeon->get_standard_level() - getMemberMaxLevel());
    // [b28d-51] 块序（N93附）反写：ORIG 0x085b5a14 段 11..19→2 落 fall-through、
    // 0..10→1 与 20+→3/0 各为岛 ⇒ 必须写成「11..19 显式区间 + 20+ 显式早返」形
    // （探针 /tmp/b28d_51_probe_abs3.cpp f7 逐条全等；f4/f5/f8/f9 均块序反）。
    if (diff < 0xb)
        return 1;
    if (diff > 0xa && diff <= 0x13)
        return 2;
    if (diff >= 0x14)
        return 3;
    return 0;
