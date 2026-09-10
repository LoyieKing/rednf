bool pvp_grade_check(CUser* user)
{
    // [b28d-solo 判词·AE] ORIG 0x0819ebde：ours 112 / ORIG 111（尾 nop 口径差 1）、
    // p167 CLEAR。修法：P48 去缓存（GetChannelType 两比较点各重取）；两段 trace 改
    // P227 全表达式临时链（ORIG 0x0819ec65/0x0819ed10 实参在 ctor 前右起求值并
    // %edi/%esi/%ebx 跨 ctor 保活，无具名槽）；IsAvailable 改 `if(!boolcall)` 正出口
    // 形（ORIG 0x0819ed09 `xor $1;test %al;je <return 1>`，P168a 技巧 1）；P258
    // 正极性 or 包裹物化「是 pvp 渠道」正条件（负极性 && 早返恒反演，b28d-46 旧形）。
    if (G_GameWorld()->GetChannelType() == 8 ||
        G_GameWorld()->GetChannelType() == 13)
    {
        int level = user->get_charac_level();
        if (level < G_CDataManager()->GetMinLevelPvPChannel() ||
            level > G_CDataManager()->GetMaxLevelPvPChannel())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x12b, 5)(
                "User Level charac_no[%d] charac_level[%d] pvpchannelLev[%d][%d]",
                user->get_charac_no(-1), level,
                G_CDataManager()->GetMinLevelPvPChannel(),
                G_CDataManager()->GetMaxLevelPvPChannel());
            return false;
        }

        if (!G_CDataManager()->IsAvailablePvPChannel(user->get_pvp_grade()))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x135, 5)(
                "User grade Error charac_no[%d] charac_pvp_grade[%d]",
                user->get_charac_no(-1), user->get_pvp_grade());
            return false;
        }
    }
    return true;
