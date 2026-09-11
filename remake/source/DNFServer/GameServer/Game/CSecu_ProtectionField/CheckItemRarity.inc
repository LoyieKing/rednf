// ORIG T 0x082887d0 _ZN21CSecu_ProtectionField15CheckItemRarityEP5CUser18SECURITY_PROTCTIONPK5CItemPK10Inven_Item
int CSecu_ProtectionField::CheckItemRarity(CUser* user, SECURITY_PROTCTION type,
                                           const CItem* item,
                                           const Inven_Item* itemRef)
{
    // b28d-L3: DIFF/P209 — ORIG 0x082887d0 对 `mov -0x18(%ebp),%eax; cmp $0x2,%eax; jg`（rarity 局部装寄存器）
    if (item == 0 || user == 0)
        return 1;
    // [M5 2026-09-11] P209/P310 试验：rarity 改 unsigned int 后 4.4 仍折
    // `cmpl $0x2,-0x18(%ebp)`（无增益，ndiff 79 不变）⇒ 回滚 int。
    int rarity = 0;
    if (item != 0 && item->get_rarity() > 1)
        rarity = item->get_rarity();

    unsigned int protType = GetProtectionType(type);
    if (protType == 0)
        return 0;
    if (itemRef == 0)
        return 1;

    // ORIG 语义（报告 CheckItemRarity.md）：低风险物品仅做账户检查；
    // 高风险（rarity>=3 / 非揭封强化≥0xb 等）走完整检查链后共享出口。
    int accountErr;
    int result;
    if (rarity < 3 &&
        (item->get_rarity() != 2 || item->IsRandomOption()) &&
        itemRef->GetUpgrade() < 0xb)   // [M5 2026-09-11] `<=0xa` 试形 4.4 仍产 `ja`
                                       // （规范化），与 ORIG 的 `jbe` 极性不可择 ⇒ 保持 `<0xb`
    {
        accountErr = CheckAccountType(user, protType);
        if (accountErr != 0)
            return accountErr;
    }
    else
    {
        result = CheckAccountType(user, protType);
        if (result != 0)
            return result;
        if (type == 0x23)
        {
            result = CheckRestoreType(user, protType);
            if (result != 0)
                return result;
            result = CheckTradingType(user, protType, type);
            if (result != 0)
                return result;
        }
        if (type != 0x29)
            return 0;
        result = CheckRestoreType(user, protType);
        if (result != 0)
            return result;
    }
    return 0;
}