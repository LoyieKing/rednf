unsigned char CUserCharacInfo::needCurCharacCheckOverEquip()
{
    if (!m_selected)
        return 0;
    return (unsigned char)CUR->m_overEquipFlag;
