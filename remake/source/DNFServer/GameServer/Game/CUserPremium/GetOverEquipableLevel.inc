int CUserPremium::GetOverEquipableLevel(ENUM_EQUIPMENTTYPE type) const
{
    // ORIG：0x4(%eax,%edx,4) 且 edx = type+0x218 → this+0x864+4*type（additionalInfo[type]）
    return m_overEquipableLevel[(int)type];
}