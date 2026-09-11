// ---------------------------------------------------------------------------
// CEquipItem::GetEmancipateMaterialList/GetEmancipateResultList —— ORIG 弱符号
// W 0x0854b528 / 0x0854b536：push ebp; mov esp,ebp; mov 0x8(%ebp),%eax;
// add $0x198/$0x1a4,%eax; pop ebp; ret —— 直返 &m_vec198 / &m_vec1a4（P1 视图）。
// 本 TU 独占定义（与 CEquipItem.h 声明配套），调用方 ProcUpgradeChronicle 使用。
std::vector<std::pair<int, int> >& CEquipItem::GetEmancipateMaterialList()
