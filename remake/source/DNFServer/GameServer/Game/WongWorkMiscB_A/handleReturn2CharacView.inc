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
