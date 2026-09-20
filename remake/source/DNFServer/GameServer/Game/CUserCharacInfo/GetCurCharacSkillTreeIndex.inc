int CUserCharacInfo::GetCurCharacSkillTreeIndex() const  // ORIG 0x822f33c _ZNK15CUserCharacInfo26GetCurCharacSkillTreeIndexEv
{
    return m_selected ? (signed char)m_selected->m_field1239 : 0;
}