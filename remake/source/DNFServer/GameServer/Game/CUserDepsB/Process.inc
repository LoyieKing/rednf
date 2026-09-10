void LevelupSupport2ndEventManger::Process(CUser* user)
{
    // b28d-solo-CUserDepsB: AE——P13 缓存 items->size()（ORIG 0x08147a8d 一次求值
    // 存 -0x10）+ P23/P52 find_item 实参内联（at(i).m_itemIndex 升格 %ebx 跨
    // G_CDataManager 调用，无具名槽）；164=164 逐条同构（余差为 .o 重定位占位）。
    // ORIG 0x08147880。
    if (user == 0)
        return;
    if (m_script.m_jobType != user->get_charac_job())
        return;
    int level = user->get_charac_level();
    std::map<int, std::vector<LevelupSupport2ndScript::ItemInfo>*>::iterator it =
        m_script.m_stepItems.find(level);
    if (it == m_script.m_stepItems.end())
        return;
    std::vector<LevelupSupport2ndScript::ItemInfo>* items = it->second;
    if (items == 0)
        return;
    const char* mailTitle = g_scriptStringManager_.findString(
        4, m_script.m_mailTitle.c_str(), (bool*)0);
    const char* mailContent = g_scriptStringManager_.findString(
        4, m_script.m_mailContent.c_str(), (bool*)0);
    // b28d-solo-CUserDepsB: P13 缓存 size()（ORIG 0x08147a8d 一次求值存 -0x10）
    // + P23/P52 find_item 实参内联（ORIG 0x08147a9f at(i).m_itemIndex 升格 %ebx
    // 跨 G_CDataManager 调用保活，无具名槽）。
    unsigned int i = 0;
    unsigned int count = items->size();
    for (; i < count; ++i)
    {
        // ORIG 用 vector::at(i)（_ZNKSt6vector…2atEj，边界检查）。
        CItem* script = G_CDataManager()->find_item(items->at(i).m_itemIndex);
        if (script == 0)
            continue;
        Inven_Item item;
        item.m_addInfo = script->get_index();      // -0x67（get_index 先于虚调用）
        script->make_item(item);                   // vtable+8
        item.set_add_info(
            static_cast<short>(items->at(i).m_itemCount));  // (short)cwtl
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            mailTitle, item, 0, user->getCurCharacNo(), mailContent,
            static_cast<int>(strlen(mailContent)), 0xf, user->GetServerGroup(),
            0, 0);
    }
