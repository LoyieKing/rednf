void ComboSkill::getComboSkillSendForm(PacketGuard& packet,
                                       ENUM_SKILL_TREE_KIND kind) const
{
    if (kind != SKILL_TREE_1 && kind != SKILL_TREE_2)
    {
        LogManager::logFormat(
            1, "ComboSkill.cpp",
            __PRETTY_FUNCTION__,
            0x99, "ComboSkill::getComboSkillSendForm kind(%d) error", kind);
    }
    else
    {
        const ComboSkillMap& curMap = getComboSkillMap(kind);
        ComboSkillMap::const_iterator it = curMap.begin();
        packet.put_byte((int)kind);
        packet.put_byte((int)curMap.size());
        for (; it != curMap.end(); ++it)
        {
            packet.put_short((int)it->first);
            packet.put_byte((int)it->second.size());
            for (unsigned int i = 0; i < it->second.size() && i < 6; ++i)
            {
                packet.put_short(it->second[i]);
            }
        }
    }
