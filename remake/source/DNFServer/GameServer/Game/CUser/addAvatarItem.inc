int CUser::addAvatarItem(unsigned long itemIdx, char param2, char param3,
                         const char* ipgAgency, eAvatarItemAddReason reason)
{
    // b28d-GrpD P183/P329 按 ORIG 0x08681224 直证：ORIG 无 G_CDataManager() 具名槽
    //   （+0x18 `call G_CDataManager` 后 %eax 直接作 this 传给 find_item），也**无**
    //   getCurCharacInvenW 具名槽（+0x9f 的 %eax 直接作 this 传 AddAvatarItem）；
    //   我方旧形多 -0x1c/-0xc 两槽（帧 0x64 vs ORIG 0x5c，并多 push esi/edi 需求）。
    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
        return -1;
    // 判据⑤（重载选择）：ORIG 0x08681224 调 `_ZNKSt6vectorI20stAvatarTypeSelect_t
    //   SaIS0_EEixEj`（const operator[]）⇒ 视图必须为 const 指针（P96 cv 旋钮）。
    const std::vector<stAvatarTypeSelect_t>* pSelect =
        (const std::vector<stAvatarTypeSelect_t>*)pItem->getAvatarTypeSelect();
    int usablePeriod = pItem->getUsablePeriod();
    if (param3 < (int)pSelect->size())
        usablePeriod = (int)(*pSelect)[param3].m_field0;
    return ((CInventory*)getCurCharacInvenW())->AddAvatarItem(
        itemIdx, usablePeriod, 0, param2, param3, ipgAgency, reason, 0, 0);
