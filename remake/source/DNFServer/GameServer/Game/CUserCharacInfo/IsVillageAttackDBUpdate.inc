// b28d-CUser P292：ORIG 0x08696976 `test;je;movzbl 0x1244;ret`（无 setne 归一化）
//   ⇒ 返回 bool 且三目两臂同型（字段已 bool 化）。
bool CUserCharacInfo::IsVillageAttackDBUpdate() const
{
    return m_selected ? CUR->m_villageAttackDBUpdate : false;
}