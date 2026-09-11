void CBoosterGage::log_booster_gage_bonus_reward(
    // [P6S3 round6 · P139 prett 轴（Main 批准，与 Q6 同轮）] ORIG 定义处 declarator
    // 用 typedef 拼写（`_ZZ...log_booster_gage_bonus_reward...E19__PRETTY_FUNCTION__`
    // @0x08b2af80 size=0x56 = `void CBoosterGage::log_booster_gage_bonus_reward(CUser*,
    // const MULTIBOX_REWARD_LIST&)`；按展开形写宏会得 0x99(153) 不符）；typedef 由 Q6
    // 落于 CBoosterGage.h:26 ⇒ 定义侧拼写同步为 typedef 名。
    CUser* pUser, const MULTIBOX_REWARD_LIST& rewardList)
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
        __PRETTY_FUNCTION__,
        0x7c, 0)("BOOSTER_GAGE : REWARD ITEM (%s) x 2 : CHARAC_NO (%u)",
                 strLog.c_str(), pUser->getCurCharacNo());
}