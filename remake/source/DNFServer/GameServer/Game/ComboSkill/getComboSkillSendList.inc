void ComboSkill::getComboSkillSendList(PacketGuard& packet) const
{
    getComboSkillSendForm(packet, SKILL_TREE_1);
    getComboSkillSendForm(packet, SKILL_TREE_2);
}