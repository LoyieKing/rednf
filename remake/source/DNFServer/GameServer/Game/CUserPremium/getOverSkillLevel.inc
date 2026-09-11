// ---------------------------------------------------------------------------
// g_SPremiumInfo 权威元素类型见 SPremiumInfo.h（stride 0x120、0x6a 元素）；
// 本 TU 经 g_SPremiumInfo[type] 具名字段访问。
// importPremiumList 终判（b28d-solo-CUserPremium）：P167 CLEAR（145/145）
// + 上列 gap ⇒ 语义完成；残余全为帧-槽族 N82/N95/P212（见函数头判词）。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CUserPremium 字段（ORIG 偏移 → CUser.h 具名成员）
//   0x000 m_entries[0x6a]  0x848 m_normalCount  0x84c m_restrictedCount
//   0x850 outFatigueRate  0x854 outPremiumCount  0x858 outExpRate
//   0x85c overEquipFlag   0x85e overSkillLevel   0x860 field860   0x862 field862
//   0x864 m_overEquipableLevel[0x58]  0x8bc m_field8bc   0x8bd m_additionalBase   0x90f m_additionalInfo
//   0x964 m_returnItemMap（ORIG 为 std::map<int,int>；真实成员，C1/D1 自动管理）
// ---------------------------------------------------------------------------
unsigned short CUserPremium::getOverSkillLevel() const
{
    return m_overSkillLevel;
}