bool CParty::CanDoPartyActionAssalutState()
{
    // ORIG：pvp_assault::ENUM_ASSAULT_STATE 型 state 存 -0xc 后双读回 reg-cmp
    // （enum 比较阻止 combine 把 load 折进 cmpl）；&& 窗口命中 inline false，
    // 两个越界判定 jle/jg 均跳尾部 mov $1。
    pvp_assault::ENUM_ASSAULT_STATE state =
        (pvp_assault::ENUM_ASSAULT_STATE)GetAssaultState();
    if (state > (pvp_assault::ENUM_ASSAULT_STATE)1 &&
        state <= (pvp_assault::ENUM_ASSAULT_STATE)6)
        return false;
    return true;
}