SkillSlot* CUserCharacInfo::getTagCharacSkillR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return (SkillSlot*)((CharacInfoFields*)m_tag)->m_skill;
}