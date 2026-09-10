void CAchievement::_gm_report(std::string& out)
{
    // ORIG 0x828d45c：stAchievement 临时对象循环外构造（get_init 直接写其 trigger 字段）
    stAchievement ach;
    // ORIG 0x828d467..0x828d47b：ach 构造之后 rep stos 清零 buf（P328⑨：
    // 初始化列表形才内联 rep stos；显式 memset 在 4.4 恒退化为 call）。
    // 循环内的 memset 在 ORIG 仍是 call（0x0828d70c），故只改首清。
    char buf[0x400] = { 0 };
    for (int idx = 0x1965; (int)idx <= 0x1bbb; ++idx)
    {
        // ORIG 0x828d489：QuestList::find_quest 死调用（结果丢弃）
        G_CDataManager()->m_questList->find_quest(idx);
        Quest* quest = G_CDataManager()->find_quest((int)idx);
        if ((quest == 0) || (quest->m_type != 7))
        {
            continue;
        }
        quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                            ach.m_trigger2);
        std::map<unsigned int, stAchievement>::iterator it =
            m_achievements.find(idx);
        // b28d-solo P167：ORIG 调 _Rb_tree_iterator::ne（!=）；写 == 生 eq 且
        // p167 记 MISSING ne + EXTRA eq。改 != 并交换两臂。
        if (it != m_achievements.end())
        {
            std::sprintf(buf,
                         "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                         quest->m_index, quest->m_name.c_str(),
                         quest->get_title_reward(), (int)ach.m_trigger0,
                         (int)ach.m_trigger1, (int)ach.m_trigger2,
                         (int)it->second.m_trigger0,
                         (int)it->second.m_trigger1,
                         (int)it->second.m_trigger2);
        }
        else
        {
            std::sprintf(buf,
                         "%d\t\"%s\"\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                         quest->m_index, quest->m_name.c_str(),
                         quest->get_title_reward(), (int)ach.m_trigger0,
                         (int)ach.m_trigger1, (int)ach.m_trigger2,
                         (int)ach.m_trigger0, (int)ach.m_trigger1,
                         (int)ach.m_trigger2);
        }
        out += buf;
        std::memset(buf, 0, sizeof(buf));
    }
