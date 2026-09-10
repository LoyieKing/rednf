void LevelUpMission::processMission(CUser* user, HeroMissionValue& value,
                                    unsigned int /*param*/)
{
    if (m_condition.m_extra[0] > user->get_charac_level())
        return;
    if (!isRepeatMission() && value.m_fieldA != 0)
        return;
    if (value.m_field8 >= user->get_charac_level())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x145,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,value:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_field8);
        return;
    }

    std::vector<HeroMissionCondition::RewardData>::iterator it =
        m_condition.m_rewards.begin();
    std::vector<HeroMissionCondition::RewardData>::iterator end =
        m_condition.m_rewards.end();
    for (; it != end; ++it)
    {
        // [b28d-solo-HeroMissionEvent] N18a 封顶：ORIG 0x08165d3b 先物化实参
        // `it->m_itemIdx`（mov (%eax),%eax; mov %eax,%ebx）再 call G_CDataManager；
        // 4.4 恒 receiver 先且折单段 mov (%eax),%ebx。P254 enum 化实测净零——
        // 字段枚举化（HeroMissionCondition.h 临时实验，已回退）使本处两段翻正，
        // 但同字段日志路径 0x08165e3a（ORIG `mov (%eax),%edi` 单段）被拖成两段，
        // gap 1→-1；P265 实参 cast 形亦无效。同字段多使用点方向相反 ⇒ 源形不可
        // 同时满足，判 N18a。
        CItem* item = G_CDataManager()->find_item(it->m_itemIdx);
        if (item == 0)
        {
            // [b28d-solo-HeroMissionEvent] ORIG 0x08165e3a 先求值 varargs
            // （edi/esi/ebx）再 call cMyTrace::C1（P227 全表达式临时链）。
            cMyTrace(__PRETTY_FUNCTION__, 0x150, 5)
                ("[HeroMission] Invalid item. (charac:%u, mission:%d, item:%u)",
                 user->get_charac_no(-1), m_missionCode, it->m_itemIdx);
            return;
        }
        Inven_Item mailItem;
        // [b28d-solo-HeroMissionEvent] P171 死写入照抄：ORIG 0x08165dbd
        // `mov (%eax),%eax; mov %eax,-0x77(%ebp)`（mailItem.m_addInfo=it->m_itemIdx，
        // 随后被 set_add_info 覆盖）。
        mailItem.m_addInfo = it->m_itemIdx;
        item->make_item(mailItem);
        mailItem.set_add_info(it->m_addInfo);
        const char* title = g_scriptStringManager_.findString(
            4, m_condition.m_mailTitle.c_str(), 0);
        const char* content = g_scriptStringManager_.findString(
            4, m_condition.m_mailContent.c_str(), 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, mailItem, 0, user->get_charac_no(-1), content,
            strlen(content), 0, user->GetServerGroup(), false, false);
    }

    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    value.m_field8 = static_cast<unsigned short>(user->get_charac_level());
    if (isRepeatMission())
    {
        value.reset();
    }
