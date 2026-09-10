void CBoosterGage::log_booster_gage_bonus_reward(
    CUser* pUser, const std::vector<std::pair<Inven_Item, bool> >& rewardList)
{
    std::string strLog;
    strLog.clear();
    for (std::vector<std::pair<Inven_Item, bool> >::const_iterator it = rewardList.begin();
         it != rewardList.end(); it++)
    {
        std::pair<Inven_Item, bool> item = *it;
        char szItem[0xff] = {0};
        sprintf(szItem, "%d ", item.first.m_addInfo);
        strLog += szItem;
    }
    cMyTrace(
        "void CBoosterGage::log_booster_gage_bonus_reward(CUser*, const MULTIBOX_REWARD_LIST&)",
        0x7c, 0)("BOOSTER_GAGE : REWARD ITEM (%s) x 2 : CHARAC_NO (%u)",
                 strLog.c_str(), pUser->getCurCharacNo());
