void CUserPremium::InitPremium()
{
    // ORIG：memset(this, 0, 0x848) —— 前缀 + entries
    memset(m_entries, 0, sizeof(m_entries));
    m_normalCount = 0;                // normalCount
    m_restrictedCount = 0;                // restrictedCount
    m_outFatigueRate = 0;              // outFatigueRate
    m_outPremiumCount = 0;                // outPremiumCount
    m_outExpRate = 0;                // outExpRate
    m_overEquipFlag = false;                        // overEquipFlag
    m_overSkillLevel = 0;              // overSkillLevel
    m_ceraDiscountRate = 0;                  // +0x860（field860）
    m_needCheckEveryday = 0;                          // +0x862（field862）
    // ORIG 0x86ad9e8 序：memset(+0x864,0x58) → +0x8bc → clear(0x8bd) →
    // clear(0x90f) → map.clear；_Additioal_info 走 **clear() 调用**（W 符号），
    // 非 memset（后者使 4.4 展开/直调 memset，与 ORIG call 形不符）
    memset(m_overEquipableLevel, 0, sizeof(m_overEquipableLevel));  // +0x864
    m_field8bc = 0;                                                 // +0x8bc
    m_additionalBase.clear();                                       // +0x8bd
    m_additionalInfo.clear();                                       // +0x90f
    m_returnItemMap.clear();                                        // +0x964
}