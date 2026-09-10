void CAchievement::_forceReset(unsigned int id)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if ((quest == 0) || (quest->m_type != 7))
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    if (isClearedAchievement(id))
    {
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(id);
        if (it != m_achievements.end())
        {
            // ORIG 0x828ceff：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
            stAchievement ach;
            if (!quest->get_init_achievement_trigger(ach.m_trigger0,
                                                     ach.m_trigger1,
                                                     ach.m_trigger2))
            {
                return;
            }
            it->second.m_trigger0 = ach.m_trigger0;
            it->second.m_trigger1 = ach.m_trigger1;
            it->second.m_trigger2 = ach.m_trigger2;
        }
        alter();
    }
    ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
    int slot = 0;
    m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
    CTitleBook::_getFitSlot(titleIdx, category, slot);
    Inven_Item* titleSlot =
        ((CTitleBook*)m_user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)14))
            ->getTitleSlot(category, slot);
    if (titleSlot == 0)
    {
        return;
    }
    // ORIG 0x828cff8：addInfo!=0 分支为 fall-through（reset），addInfo==0 分支 je 跳库存检查
    if (titleSlot->m_addInfo != 0)
    {
        titleSlot->reset();
    }
    else
    {
        Inven_Item* invenItem =
            ((CInventory*)m_user->getCurCharacInvenW())->GetInvenRef(0, 0xb);
        // ORIG 0x0828d032 `je +0x247`（invenItem==0 落**独立** nop 岛）与
        // 0x0828d043 `jne +0x248`（addInfo 不等**直落** epilogue）落点不同 ⇒
        // 两条独立守卫 + 尾标签（P133①/P223 多源 goto 尾标签）：
        // 多源同目标 goto 由 4.4 逐条发射，不 jump-thread。
        if (invenItem == 0)
            goto force_reset_end;
        if (invenItem->m_addInfo == titleIdx)
        {
            invenItem->reset();
            m_user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, 0xb);
        }
    }
force_reset_end:;
