void SkillSlot::getComboSkillSendInfo(PacketGuard& packet) const
{
    m_comboSkill.getComboSkillSendList(packet);
}