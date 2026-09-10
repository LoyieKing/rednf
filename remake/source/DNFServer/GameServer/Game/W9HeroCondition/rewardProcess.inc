bool BaseHeroMissionCondition::rewardProcess(CUser* user)
{
    for (std::vector<HeroMissionCondition::RewardData>::iterator it =
             m_condition.m_rewards.begin(),
         end = m_condition.m_rewards.end();
         it != end; ++it)
    {
        // ORIG 0x0816507e：pt() → ebx（跨 G_CDataManager 调用缓存，P52），
        // G_CDataManager() 直返调用 find_item（无判空，P15 形态）。
        CItem* found = G_CDataManager()->find_item(it.operator->()->m_itemIdx);
        if (found == 0)
        {
            // ORIG 0x081650a3..0x08165109：临时链式 cMyTrace（P37/f2 探针实证：
            // operator() 实参右→左先求值并 edi/esi/ebx 缓存，然后才构造临时）。
            cMyTrace(__PRETTY_FUNCTION__,
                     0x6e, 5)(
                "[HeroMission] Invalid item. (charac:%u, mission:%d, item:%u)",
                user->get_charac_no(-1), static_cast<int>(m_missionCode),
                it->m_itemIdx);
            return false;
        }

        // ORIG 0x08165119..0x08165162：Inven_Item ctor → m_addInfo = reward 低 half
        // → make_item(vtable 槽) → set_add_info(ushort@reward+4)。
        Inven_Item item;
        item.m_addInfo = it->m_itemIdx;
        found->make_item(item);
        item.set_add_info(static_cast<int>(it->m_addInfo));

        // ORIG 0x08165169..0x08165233。
        short slot = -1;
        slot = static_cast<short>(
            user->getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, true));
        if (slot >= 0)
        {
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0,
                                 static_cast<int>(slot));
        }
        else
        {
            // ORIG 0x08165238..0x081652c9：插入失败时按 mailTitle/mailContent 经
            // findString(table 4) 解析，发系统邮件兜底。
            const char* mailTitle =
                g_scriptStringManager_.findString(
                    4, m_condition.m_mailTitle.c_str(), 0);
            const char* mailContent =
                g_scriptStringManager_.findString(
                    4, m_condition.m_mailContent.c_str(), 0);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                mailTitle, item, 0,
                static_cast<unsigned int>(user->get_charac_no(-1)),
                mailContent, static_cast<unsigned int>(strlen(mailContent)),
                0, user->GetServerGroup(), false, false);
        }
    }
    return true;   // ORIG 0x08165331：循环正常结束返回 1。
