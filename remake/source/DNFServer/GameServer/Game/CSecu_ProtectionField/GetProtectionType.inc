unsigned int CSecu_ProtectionField::GetProtectionType(SECURITY_PROTCTION type)
{
    // ORIG T 0x082880dc（报告 GetProtectionType.md）：map 查找，?: 布局。
    std::map<SECURITY_PROTCTION, unsigned int>::iterator it =
        m_typeMap.find(type);
    return it != m_typeMap.end() ? it->second : 0u;
