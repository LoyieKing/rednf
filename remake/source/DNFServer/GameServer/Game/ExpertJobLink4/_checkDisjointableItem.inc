// ---- DisPatcher_DisJointItem::_checkDisjointableItem
// （报告 _checkDisjointableItem.md，ORIG T 0x081fa0f6）：
// 未鉴定且无目标用户 → 0xae；物品缺失 → 0x11；
// IsImpossibleDisjoint（ORIG vptr+0x38 = vtable 槽16）→ 0x13；GetAttachType==2 → 0x11。 ----
// [b28c-31] N7+N18b：虚调用 callee ORIG %edx vs 4.4 %eax（vptr+0x38 槽）；
// 另 GetAttachType 结果 ORIG 回读槽再 `cmp $0x2,%eax`，4.4 恒折 `cmpl $0x2,-0xc`
// 内存直比（P209 附 / N18b）。
int DisPatcher_DisJointItem::_checkDisjointableItem(const Inven_Item& item,
                                                    CUser* targetUser)
{
    // ORIG 0x081fa0fd..0x081fa12d：item.m_amp(+0x11).isIdentified()==false
    // 且 targetUser==0 → 0xae（未鉴定且无目标用户不可分解）
    if (!item.m_amp.isIdentified() && targetUser == 0)
        return 0xae;

    CItem* itemObj = G_CDataManager()->find_item(item.m_addInfo);
    if (itemObj == 0)
        return 0x11;

    if (itemObj->IsImpossibleDisjoint())   // ORIG vptr+0x38（IsImpossibleDisjoint，非 CharacCargo）
        return 0x13;
    // b28d-61 [P286 单 case switch 复现寄存器比较，ORIG 0x081fa183
    // `mov -0xc(%ebp),%eax; cmp $0x2,%eax; jne`]：if 形（int/unsigned/const/cast
    // 六形探针 /tmp/b61_probe_att{,2}.cpp）4.4 恒折 `cmpl $0x2,-0xc` 内存直比；
    // 单 case switch 得寄存器链。
    int attachType = itemObj->GetAttachType();
    switch (attachType)
    {
    case 2:
        return 0x11;
    }
    return 0;
}