void CHandlePremium::handleRemoveOneDayLethePremium(CUser* user)
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)0x21, 0, 0, 0, true);
        user->RemoveRestrictedGoods(
            G_CDataManager()->m_itemList->GetRestrictCode(0xc5));
        handleNotifyPremiumInfo(user, 0x21, 0);
    }
    user->SetOneDayLetheFlag(CPremiumLetheManager::NOT_USE_LETHE_STATE);

    cMyTrace(__PRETTY_FUNCTION__, 0x3b4, 0)(
        "ONE_DAY_LETHE : PREMIUM_END, char(%s)", user->getCurCharacName());
