// [b28d-L14 N107+控制流] ORIG 0x08680e04：`lea 0xdaa(%edx),%ecx` vs 4.4 `add $0xdaa,%eax`
// （N107 成员常量偏移取址六形全败）；否定早返布局（P59/P222）。p167 CLEAR（6/6）。
// [b28d-solo-CUserCharacInfo AE] 85=85：①守卫独立早返（P59）已落；②CCargo/CInventory::
// FindItemLock bool 化（Main 代落）；③N107 两处 add-vs-lea 由 P106 真实类型成员取址
// 归零 —— m_cargo 由 char[0x14] 改真实 CCargo（CUserCharacInfo.h），m_inven 经 pack(1)
// CharacInvenView 真实 CInventory 成员访问 ⇒ `lea 0xdaa(%edx),%ecx` / `lea 0xda(%edx),%ecx`
// 逐条复现（数组退化形恒产 add）。
bool CUserCharacInfo::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot,
                                   CUser* user) const
{
    if (user == 0)
        return false;
    if (m_selected == 0)
        return false;
    if (m_selected->m_cargo.FindItemLock(b, space, slot))
        return true;
    if (m_selected->m_curCharacInven.FindItemLock(b, space, slot))
        return true;
    CExpandEquipslot* expand = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    if (expand->FindItemLock(b, space, slot))
        return true;
    if (((CTitleBook*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14))
            ->findItemLock(b, space, slot))
        return true;
    return false;
}