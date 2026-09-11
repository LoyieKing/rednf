// ---- ProcessEquipItem —— T 0x082714c5（同名报告全文转写；委托转写核对）-----
// 与 ProcessInsertItem 仅差 SetUpgrade/rentId 写入顺序、装备位插入与标记字节。
// [b28d-18 N71] 同 ProcessInsertItem：空析构 trivial 判定差异（P71） | ORIG 0x082714c5
// [b28d-solo-GlobalData] DIFF 封顶：p167 逐调用同形；残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] P71 作废（同 RentalInfo 伪析构翻案）；0x082714c5 全切片
// 重对齐：①删 savedRentId/slot 具名（ORIG 无 -0x10 槽、`mov %ax,-0x1a` 直存）；
// ②ri 写序按 ORIG = slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId；
// ③**语义修正**：m_field8/9/B 逐字节取自 **inElem**[0]/[1]/[3]
//   （0x08271610/619/623 `mov 0xc(%ebp); movzbl (1,%eax)` 直证，旧 `reward>>8/>>24`
//   为误推断）；④rentId 两处 if/else 直存形（ORIG cmp/je+jmp，无 setcc 物化）。
int ProcessEquipItem(CUser& user,
                     const InstanceRentalSystem::RentInElem& inElem,
                     const EventRewardItemInfo& reward,
                     InstanceRentalSystem::RentOutput& out, int reason)
{
    // [b28d-PoolGlobalData] 余 1 条 = ORIG 0x082714d2 `mov (%eax),%eax;
    // mov %eax,%ebx` 两段装载（4.1 regmove），4.4 恒直落 %ebx；`register` 提示
    // 形实测反劣（ndiff 3→20：ebx 长命化引 push %esi/帧 0xb0/复用 ebx 少两读）
    // ⇒ 回退具名形，N18a 封顶。
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    // ORIG 0x08271526：装备路径先写 rentId（-0x52=item+7），后 SetUpgrade。
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }
    item.SetUpgrade((unsigned char)reward.m_short8);

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoEquipment(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 3;                                   // 装备租借标记
    ri.m_inEquip = 1;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x0827162a：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
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