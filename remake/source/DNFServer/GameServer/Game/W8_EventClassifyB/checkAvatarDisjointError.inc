// checkAvatarDisjointError（ORIG 0x0810f3a4）
// get_grade 两次独立调用（P48：ORIG +0x059/+0x068 双 call）。
// 注意：ORIG 虚调用 getUsableItemType 在 vtable+0x24，当前 CItem.h 虚序生成
// +0x20 —— 共享头（CItem.h）虚函数序与 ORIG vtable 差 2 槽，需全局修复，
// 见报告；此处保持头现状。
int CAvatarDisjointRandomReward::checkAvatarDisjointError(
    CUser* user, unsigned short slot, Inven_Item& item, CItem const* citem)
{
    // [b28c-L0 NEAR·N82附] 槽深差，语义全同。
    const int enable = user->isEnableAvatarSocketAction();
    if (enable != 0)
        return enable;

    if (user->CheckItemLock(2, (int)slot))
        return 0xd5;

    if (citem->get_grade() == 0 || citem->get_grade() > 3)
        return 0x13;

    // [Main b28d 修正] Ghidra 原码为 vtable+0x24 的间接调用（arg=2，返回值 ==1 才继续）。
    // 按 Itanium 槽序（vptr = _ZTV+8）：vptr+0x24 = _ZTV+0x2c = word[11] =
    // **isEnableAvatarFunc**（CItem 虚序：… IsClearAvatar[8] IsEnchantalbe[9]
    // getUsableItemType[10] isEnableAvatarFunc[11] …；CEquipItem/探针 vtable 双侧逐槽一致）。
    // 旧重建误认成 getUsableItemType（那是 vptr+0x20）⇒ 运行时调错虚函数，已改正。
    if (!citem->isEnableAvatarFunc((ENUM_AVATAR_FUNC_FILTER)2))
        return 0x17;

    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(
            item.m_addInfo2) != 0)
        return 0x17;

    // ORIG：isfindEventAvatar(G_CDataManager()+0xb508=CDataManager::
    // m_eventAvatarList, item.m_addInfo)
    if (G_CDataManager()->m_eventAvatarList.isfindEventAvatar(
            item.m_addInfo))
        return 0x17;

    if (item.m_upgradeSep.IsTradeRestriction())
        return 0x17;
    return 0;
}