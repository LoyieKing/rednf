// ---------------------------------------------------------------------------
// CHandlePremium::handleReturn2CharacView —— T 0x086af96e。
// 取 premium 列表(type 上限 0x270f)，逐项：type!=0 且 state==3(已过期) 时以
// handleSetUserPremium(type,0,0,0,true) 重设（链路收尾重算）。
void CHandlePremium::handleReturn2CharacView(CUser* user)
{
    std::vector<SUserPremiumInfo> list =
        user->GetPremiumInfo()->GetPremiumInfoList(0x270f);
    for (std::size_t i = 0; i < list.size(); ++i)
    {
        if (list[i].m_type != 0 && list[i].m_state == 3)
            handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)list[i].m_type,
                                 0, 0, 0, true);
    }
}