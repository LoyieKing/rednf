// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681224 | _ZN5CUser13addAvatarItemEmccPKc20eAvatarItemAddReason
int CUser::addAvatarItem(unsigned long itemIdx, char param2, char param3,
                         const char* ipgAgency, eAvatarItemAddReason reason)
{
    // b28d-GrpD P183/P329 按 ORIG 0x08681224 直证：ORIG 无 G_CDataManager() 具名槽
    //   （+0x18 `call G_CDataManager` 后 %eax 直接作 this 传给 find_item），也**无**
    //   getCurCharacInvenW 具名槽（+0x9f 的 %eax 直接作 this 传 AddAvatarItem）；
    //   我方旧形多 -0x1c/-0xc 两槽（帧 0x64 vs ORIG 0x5c，并多 push esi/edi 需求）。
    // 2026-09-11（H3CUser）：ORIG 0x08681224 有**两个**具名量槽——`-0x28`
    //   （find_item 结果，`cmpl $0,-0x28` 判空）与 `-0x24`（`mov -0x28,%eax;
    //   mov %eax,-0x24` 拷贝，后续 getAvatarTypeSelect/getUsablePeriod 的 this 均
    //   从 -0x24 装载）。我方旧形单槽 -0x24 且判空直用 ⇒ 少 2 条 mov、
    //   后续槽整体上移 4（-0x20/-0x1c vs ORIG -0x20/-0x1c 恰好同，因 ORIG 多占
    //   一槽而起始更深）。补第二个具名局部复现。
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
        return -1;
    CEquipItem* pEquip = (CEquipItem*)pItem;
    // 判据⑤（重载选择）：ORIG 0x08681224 调 `_ZNKSt6vectorI20stAvatarTypeSelect_t
    //   SaIS0_EEixEj`（const operator[]）⇒ 视图必须为 const 指针（P96 cv 旋钮）。
    const std::vector<stAvatarTypeSelect_t>* pSelect =
        (const std::vector<stAvatarTypeSelect_t>*)pEquip->getAvatarTypeSelect();
    int usablePeriod = pEquip->getUsablePeriod();
    if (param3 < (int)pSelect->size())
        usablePeriod = (int)(*pSelect)[param3].m_field0;
    return ((CInventory*)getCurCharacInvenW())->AddAvatarItem(
        itemIdx, usablePeriod, 0, param2, param3, ipgAgency, reason, 0, 0);
}