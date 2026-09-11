// ---- ProcessInsertItem —— T 0x08271666（同名报告全文转写）------------------
// [b28d-18 N71] EXTRA RentalInfo::D1Ev×2 + _Unwind_Resume×1：ORIG 空析构被 4.1 判 trivial 不调用（P71；throw() 实测无效） | ORIG 0x08271666
// [b28d-solo-GlobalData] DIFF 封顶（已被二压推翻）：N71 判词倒果为因。
// [b28d-PoolGlobalData 二压] ①删 `~RentalInfo(){}` 伪建模（ORIG `_Destroy_aux<ILb1E>`
// 实证平凡）⇒ EXTRA D1Ev×2+_Unwind_Resume×1 归零；②同 0x082714c5 全切片重对齐：
// 删 savedRentId/slot 具名、ri 写序 slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId、
// **语义修正** m_field8/9/B=inElem[0]/[1]/[3]（0x082717b1/7ba/7c4 `movzbl` 直证）。
int ProcessInsertItem(CUser& user,
                      const InstanceRentalSystem::RentInElem& inElem,
                      const EventRewardItemInfo& reward,
                      InstanceRentalSystem::RentOutput& out, int reason)
{
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    item.SetUpgrade((unsigned char)reward.m_short8);  // 插入路径：先升级后 rentId
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoInventory(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 0;                                   // 背包租借标记
    ri.m_inEquip = 0;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x082717cb：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
    if (inElem.m_rentItemId != -1)
    {
        ri.m_rentItemId = inElem.m_rentItemId;
    }
    else
    {
        ri.m_rentItemId = item.m_addInfo2;
    }

    out.m_infos.push_back(ri);
    return 0;
}