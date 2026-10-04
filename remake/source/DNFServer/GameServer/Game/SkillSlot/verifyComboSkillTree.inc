void SkillSlot::verifyComboSkillTree(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != 0 && kind != 1)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__, 0xd33,
            "SkillSlot::verifyComboSkillTree kind(%d) error", kind);
    }
    else
    {
        std::vector<std::pair<int, int> >* slotList =
            &((CCharacter*)G_CDataManager()->m_characList)[9].m_giveSkillList[0];
        for (int i = 0; i < 6; ++i)
        {
            if ((*slotList)[i].first == 0)
            {
                continue;
            }
            m_comboSkill.eraseDuplicationComboSKill(
                kind, (*slotList)[i].first);
        }
    }
}