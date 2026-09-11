bool CMission::isAcceptCondition(const CUser& user) const
{
    // [b28d-70] 判词：AE 78=78（m_rankRange.first/.second、m_rateRange.first/.second
    // 偏移 +0x18/+0x1c/+0x20/+0x24 与原字段名完全同址，pair 化零指令差）。
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->getIndex_byKind(GetMissionKind()) != 0) return 0;

    // b28d-50：ORIG 0x085e31d9 段无 grade 槽（两次独立 call get_pvp_grade，
    // 0x085e31f0/0x085e31ff）且布尔**留在 %al**（mov $1/$0 + test，无槽）⇒ 原码是
    // 单一 `if (A && (B || C)) return 0;`（&&/|| 跳转物化），非具名 bool + else-if；
    // m_rankRange.first 作左操作数先入 %ebx 跨 call 保活（P23/P66）。
    if ((m_rankRange.first != -1) &&
        ((m_rankRange.first > user.get_pvp_grade()) ||
         (user.get_pvp_grade() >= m_rankRange.second)))
        return 0;

    int winRate = user.get_pvp_WinningRate_relateMission();
    // b28d-50：ORIG 0x085e3233 段块序为 REJECT（mov $0）在前、PASS（虚调）在后
    // （je PASS; jg REJECT; jg PASS; REJECT; PASS）⇒ 原码为否定式 `if(拒绝) return 0;
    // return _isAcceptCondition(user);`，而非正向 PASS 先行（4.4 反产 jle REJECT）。
    if ((m_rateRange.first != -1) &&
        ((m_rateRange.first > winRate) || (winRate >= m_rateRange.second)))
        return 0;
    return _isAcceptCondition(user);
}