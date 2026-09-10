void CAchievement::_forceClear(unsigned int id)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    // ORIG 0x0828ce5c/0x0828ce69：`je +0x238` 与 `jne +0x23b` **落点不同**
    // （各带独立尾岛）⇒ 两条独立守卫（P133①），合并短路会共落点少一岛。
    if (quest == 0)
    {
        return;
    }
    if (quest->m_type != 7)
    {
        return;
    }
    int titleIdx = quest->get_title_reward();
    if (titleIdx == 0)
    {
        return;
    }
    // ORIG 0x828ccbb：isClearedAchievement 结果 xor 1 后 je 到已清除分支，
    // 未清除分支（重新初始化 trigger+发称号）为 fall-through，故源码用 !cleared 先行。
    if (!isClearedAchievement(id))
    {
        // ORIG 0x828cccc：stAchievement 临时对象 ctor 清零，get_init 覆盖 trigger 字段
        stAchievement ach;
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        _setTrigger(id, ach.m_trigger0, ach.m_trigger1, ach.m_trigger2);
        CTitleBook::putTitleOnly(m_user, titleIdx);
    }
    else
    {
        ENUM_TITLE_BOOK_CATEGORY category = (ENUM_TITLE_BOOK_CATEGORY)4;
        int slot = 0;
        m_user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14);
        CTitleBook::_getFitSlot(titleIdx, category, slot);
        // b28d-solo P0/P278：ORIG 0x0828cd6x 无 book/inven 具名槽
        //（GetCharacExpandData 结果经 %edx 直传 getTitleSlotR；getCurCharacInvenR
        //  结果经 %eax 直传 GetInvenSlot），改匿名全表达式链。
        Inven_Item titleSlot =
            ((CTitleBook*)m_user->GetCharacExpandData(
                 (ENUM_CHARAC_EXPAND_TYPE)14))
                ->getTitleSlotR(category, slot);
        if (titleSlot.m_addInfo == 0)
        {
            // ORIG 0x828cdcc 调 _ZNK15CUserCharacInfo18getCurCharacInvenREv（只读引用）
            // [b28d-D33 P215] ORIG 0x0828cc56 段无 Inven_Item::C1Ev call
            // （p167 差集 EXTRA +1 C1Ev 实证）⇒ 原码是直接初始化，非「先声明后赋值」。
            Inven_Item invenItem = m_user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
            if ((invenItem.m_addInfo == 0) ||
                (invenItem.m_addInfo != titleIdx))
            {
                CTitleBook::putTitleOnly(m_user, titleIdx);
            }
        }
    }
    alter();
