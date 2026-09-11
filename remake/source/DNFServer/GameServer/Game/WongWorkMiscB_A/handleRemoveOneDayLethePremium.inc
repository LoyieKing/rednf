// ---------------------------------------------------------------------------
// CHandlePremium::handleRemoveOneDayLethePremium —— T 0x086b0594。
// isAffectedPremium(0x21) 时撤销该 premium、取限制物品码并移除受限物品、通知；
// 无条件置非使用 Lethe 状态并 trace 角色名。
// [b28d-solo-WongWorkMiscB_A] 两处修正：
//   ① `getItemList()` 为头内 inline，4.4 -O0 仍发 out-of-line call（ORIG 4.1 内联为
//      `mov 0xc(%eax),%eax`）⇒ 调用点改直访公有成员 `m_itemList`（p167 EXTRA
//      `CDataManager::getItemList()` 归零）。
//   ② ORIG 尾段为 P227 全表达式临时链：`call getCurCharacName; mov %eax,%ebx;`
//      跨 cMyTrace ctor 保活，ctor 后才压 op() 实参 ⇒ 具名 trace 改临时链 +
//      `__PRETTY_FUNCTION__`（P139：ORIG `_ZZ…E19__PRETTY_FUNCTION__`）。
void CHandlePremium::handleRemoveOneDayLethePremium(CUser* user)
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
        handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)0x21, 0, 0, 0, true);
        user->RemoveRestrictedGoods(
            G_CDataManager()->m_itemList->GetRestrictCode(0xc5));
        handleNotifyPremiumInfo(user, 0x21, 0);
    }
    user->SetOneDayLetheFlag(CPremiumLetheManager::NOT_USE_LETHE_STATE);

    cMyTrace(__PRETTY_FUNCTION__, 0x3b4, 0)(
        "ONE_DAY_LETHE : PREMIUM_END, char(%s)", user->getCurCharacName());
}