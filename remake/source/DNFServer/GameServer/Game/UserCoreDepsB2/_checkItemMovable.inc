// ---------------------------------------------------------------------------
// 4. _checkItemMovable —— ORIG T 0x0865f1e1（报告 _lt_global_gt_/_checkItemMovable.md）
// exchange_interspace(_expand) 前置校验：源/目标空间为 2 或 0xc 直接放行；
// 否则按 INVEN_TYPE 取槽内 item idx（5=扩充栏 9 号页 / 6=金库期望清零 /
// 其余走背包），两侧 idx 均与期望一致才允许移动。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] P0 去 inven 具名局部（ORIG else 分支把
// getCurCharacInvenR() 内联进实参，无该槽）+ foundFrom/foundTo 零初始化合并到
// 分支前（ORIG `movl $0,-0x18; movl $0,-0x14` 连续）：116→112（ORIG 114），帧
// $0x128=ORIG $0x124+push ebx、标量槽 -0x20/-0x1c/-0x18/-0x14 与 ORIG 逐位一致。
// 残 2 条 = ①N93 四个块级 sret 临时槽分配方向相反（ORIG 首分支 -0x114 最深→
// -0x5d 最浅，4.4 反向）；②N7 else 分支 fromType 4.1 升 %ebx 跨 receiver call
// 保活（push %ebx/pop），4.4 恒 mem→%ecx（P226 语句表达式、裸 register 双旋钮实测：
// 前者无效、后者过度升格为 %esi+%ebx 且帧 0x120/异常 epilogue 反劣，已回退）。
// 语义闭合（ORIG 0x0865f1e1）。
bool _checkItemMovable(const CUser* user, ENUM_ITEMSPACE fromSpace, int fromSlot,
                       unsigned long fromItemIdx, ENUM_ITEMSPACE toSpace, int toSlot,
                       unsigned long toItemIdx)
{
    if (fromSpace == (ENUM_ITEMSPACE)2 || toSpace == (ENUM_ITEMSPACE)2)
        return true;
    if (fromSpace == (ENUM_ITEMSPACE)0xc || toSpace == (ENUM_ITEMSPACE)0xc)
        return true;

    int fromType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)fromSpace);
    int toType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)toSpace);

    int foundFrom = 0;
    int foundTo = 0;
    if (fromType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)fromType, fromSlot);
        foundFrom = slot.m_addInfo;                     // 读 sret 结果 +2
    }
    else if (fromType == 6)
    {
        fromItemIdx = 0;                                // ORIG 清参数槽 0x14(%ebp)
    }
    else
    {
        Inven_Item slot =
            user->getCurCharacInvenR()->GetInvenSlot(fromType, fromSlot);
        foundFrom = slot.m_addInfo;
    }

    if (toType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)toType, toSlot);
        foundTo = slot.m_addInfo;
    }
    else if (toType == 6)
    {
        toItemIdx = 0;                                  // ORIG 清参数槽 0x20(%ebp)
    }
    else
    {
        Inven_Item slot =
            user->getCurCharacInvenR()->GetInvenSlot(toType, toSlot);
        foundTo = slot.m_addInfo;
    }

    return foundFrom == (int)fromItemIdx && foundTo == (int)toItemIdx;
}