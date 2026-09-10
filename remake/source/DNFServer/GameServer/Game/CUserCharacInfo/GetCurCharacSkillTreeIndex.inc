int CUserCharacInfo::GetCurCharacSkillTreeIndex() const  // ORIG 0x822f33c _ZNK15CUserCharacInfo26GetCurCharacSkillTreeIndexEv
{
    return m_selected ? (signed char)CUR->m_skillTreeIndex : 0;
