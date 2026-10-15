// ORIG 0x868340a：与 ItemAdd 对应的删除日志（无 XPos/YPos，11 个 %d）；
// 尾部 reason==8 且 ITEM_TYPE==1 时补 "Compound-" 日志。
// [P329 本泳道 2026-09-09] ndiff 584 → 14。分支树原本即正确（`if{if{A}}else{B}` + 独立
// Compound 块），单靠 P329 收敛：
//   改前：unsigned int reseal = item.GetReSealCount(); unsigned int upgrade = item.GetUpgrade();
//         register unsigned int lockV = item.m_amp.GetLock(); ... typeV/fieldV/sepV/addInfoV/upgradeV
//         LogHistory(fmt, ..., upgrade, ..., reseal, Print()(guid));   // Compound 同理 7 个具名
//   改后：上述 getter 全部内联回各自实参位（Compound 块 8 个实参全内联）。
// 机理同 ItemAdd：ORIG 自末实参起求值，具名化使求值块/装载块错位。
// [2026-09-09] N102 已翻案（`*(0, m_traderCharac)`）；Compound 段 `%0x1f` 掩码改位域
// m_bits.m_lo 复现 ORIG 的「and 后截断」形（探针 mask2 w1）⇒ 残差仅 N107 ×2（同 ItemAdd）。
// [SweepHistoryLog2 2026-09-09 复核] 同上 12 形复验（引用形参恒 add、指针形参才 lea）⇒ 维持封顶。
void cUserHistoryLog::ItemDel(INVEN_TYPE invenType, int a3, int a4,
                              const Inven_Item& item, eItemDelReason reason)
{
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (*(0, m_traderCharac) != 0)
        {
            m_user->LogHistory(
                "Item-,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                item.GetUpgrade(), (unsigned)item.m_endurance,
                item.m_random.m_random.m_slots[0].m_optionIndex, item.m_random.m_random.m_slots[0].m_firstValue, item.m_random.m_random.m_slots[0].m_secondValue, item.m_random.m_random.m_slots[1].m_optionIndex, item.m_random.m_random.m_slots[1].m_firstValue, item.m_random.m_random.m_slots[1].m_secondValue,
                item.m_random.m_random.m_slots[2].m_optionIndex, item.m_random.m_random.m_slots[2].m_firstValue, item.m_random.m_random.m_slots[2].m_secondValue, item.m_random.m_random.m_gradeSeed.m_value, item.m_random.m_random.m_changedOption.m_optionIndex, item.m_random.m_random.m_changedOption.m_firstValue,
                item.m_random.m_random.m_changedOption.m_secondValue, item.m_random.m_random.m_changedSlot.m_value,
                item.m_seal,
                item.GetReSealCount(),
                CItemGloballyUniqueIdentifierPrint()(
                    *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
        }
    }
    else
    {
        m_user->LogHistory(
            "Item-,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            item.m_addInfo2, item.m_extendInfo,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            item.GetUpgrade(), (unsigned)item.m_endurance,
            item.m_random.m_random.m_slots[0].m_optionIndex, item.m_random.m_random.m_slots[0].m_firstValue, item.m_random.m_random.m_slots[0].m_secondValue, item.m_random.m_random.m_slots[1].m_optionIndex, item.m_random.m_random.m_slots[1].m_firstValue, item.m_random.m_random.m_slots[1].m_secondValue,
            item.m_random.m_random.m_slots[2].m_optionIndex, item.m_random.m_random.m_slots[2].m_firstValue, item.m_random.m_random.m_slots[2].m_secondValue, item.m_random.m_random.m_gradeSeed.m_value, item.m_random.m_random.m_changedOption.m_optionIndex, item.m_random.m_random.m_changedOption.m_firstValue,
            item.m_random.m_random.m_changedOption.m_secondValue, item.m_random.m_random.m_changedSlot.m_value,
            item.m_seal,
            item.GetReSealCount(),
            CItemGloballyUniqueIdentifierPrint()(
                *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
    }

    // Compound 补丁块（ORIG +0x543）：reason==8 && m_field1(ITEM_TYPE)==1。
    if ((int)reason == 8 && item.m_itemType == 1)
    {
        // ORIG 0x086839xx：实参位右到左即时求值（P44）——全部实参就地内联。
        m_user->LogHistory(
            "Compound-,%u,%d,%d,%d,%d,%d,%d,%d,",
            (unsigned)item.m_addInfo, item.GetUpgrade(), item.get_add_info(),
            // [b28d-solo-cUserHistoryLog 2026-09-09] ORIG 0x086839xx 读形为
            // `movzbl 0x33(%eax),%eax; and $0x1f,%eax; movzbl %al,%eax`（and 后截断）；
            // 写 `m_field & 0x1f` 时 4.4 把截断提到 and 之前并经 %edx 中转（多 1 条）。
            // 位域 m_lo 读取即 ORIG 形（探针 /tmp/hlprobe/mask2.cpp w1 逐条同形）。
            item.m_upgradeSep.m_bits.m_upgradeSeparate, item.m_extendInfo,
            item.m_amp.getAbilityType(), item.m_amp.getAbilityValue(),
            item.m_amp.GetLock());
    }
}