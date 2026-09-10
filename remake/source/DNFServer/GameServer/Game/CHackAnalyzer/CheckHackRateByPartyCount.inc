bool CHackAnalyzer::CheckHackRateByPartyCount(ENUM_HACKTYPE type, int count)
{
    // ORIG 0x080f8320：std::find 于 m_checkRateHackTypes(+0x7a24) 查 type；
    // 未命中返回 true；idx=count-1 越界或费率==100 返回 true；
    // 否则命中当 get_rand_int(100) <= 费率。
    std::vector<int>::const_iterator it = std::find(
        m_checkRateHackTypes.begin(), m_checkRateHackTypes.end(), (int)type);
    if (it == m_checkRateHackTypes.end())
        return true;

    int idx = count - 1;
    if (idx < 0 || idx > 3 || _S_HACK_CHECK_RATE_BY_PARTYCOUNT[idx] == 100)
        return true;
    // ORIG 尾段 if 常量岛（同 CheckHackRateByDifficult b27 判定）。
    if (get_rand_int(100) <= _S_HACK_CHECK_RATE_BY_PARTYCOUNT[idx])
        return true;
    return false;
