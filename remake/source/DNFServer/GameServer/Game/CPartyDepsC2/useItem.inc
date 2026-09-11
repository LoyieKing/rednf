// ---------------------------------------------------------------------------
// useItem —— ORIG T 0x085a7800
// 报告：docs/class_func_reports/CParty/useItem.md
// ---------------------------------------------------------------------------
void CParty::useItem(CUser* user, Inven_Item const& item)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)->onUseItem(user, item);
}