// ---------------------------------------------------------------------------
// CUser::GetCurCharacInventoryRef（ORIG T 0x08680f2e 报告全文已读）
// switch 跳表区域编码：case0→GetInvenRef(1)、case1→GetInvenRef(2)、
// case2→GetCargoRef、case3→GetInvenRef(0)、case7→GetInvenRef(3)、
// case0x12→GetCharacExpandData(kServerItemExpansion=9).GetInvenRefW(5)、
// case0x13..0x16→GetCharacExpandData(kTitleBook=0xe).getTitleSlot(area-0x13)。
// ORIG 共享出口 +0x172；default/guard 均 mov $0 进同一出口。
// ---------------------------------------------------------------------------
Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short area,
                                            unsigned short slot)
{
    // b28d-65：ORIG 0x08680cab `test;setne;test;je` = 正极性 `if (getCurCharacR())`
    // 物化形（P292 表）；负极性 `==0` 在 4.4 生 `sete`。改正向包裹、尾部统一 return 0。
    if (getCurCharacR())
    {
        switch (area)
        {
        case 0:
            return getCurCharacInvenW()->GetInvenRef(1, slot); // [0x868062 GetInvenRefEii]
        // b28d-65 + [2026-09-11 更正]：本件（非 const 版 0x08680f2e）ORIG 叶块排放序
        // = 0,2,3,**1**,**7**,0x12,0x13..0x16（0x8680f85 $1 / 0x8680fa4 GetCargoRef /
        //  0x8680fcd $0 / 0x8680ff5 $2 / 0x868101d $3 / 0x868102f $9 / 0x868106f $0xe）
        // ⇒ case 1 在 case 7 **之前**（与 const 版 0x08680c8a 相反，勿混用）。
        case 2:
            return getCurCharacCargoW()->GetCargoRef(slot);    // [0x8680fa4 GetCargoRefEi]
        case 3:
            return getCurCharacInvenW()->GetInvenRef(0, slot); // [0x8680fba]
        case 1:
            return getCurCharacInvenW()->GetInvenRef(2, slot); // [0x8680f9a]
        case 7:
            return getCurCharacInvenW()->GetInvenRef(3, slot); // [0x868100a]
        case 0x12:
        {
            // b28d-65 [P0]：ORIG 0x08680d99 段把 GetCharacExpandData 结果存槽 -0xc 再重载
            // （槽往返 ⇒ 原码有具名局部），链式写法会让 4.4 改把 slot 存 %ebx 跨调用。
            CExpandEquipslot* pExpand = GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
            return pExpand->GetInvenRefW(static_cast<INVEN_TYPE>(5), slot); // [0x868102f]
        }
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
            return reinterpret_cast<CTitleBook*>(GetCharacExpandData(
                       static_cast<ENUM_CHARAC_EXPAND_TYPE>(14)))
                       ->getTitleSlot(
                           static_cast<ENUM_TITLE_BOOK_CATEGORY>(area - 0x13),
                           slot);                             // [0x8681062 ebx=area-0x13]
        default:
            return 0;                                          // [0x8681094 mov $0]
        }
    }
    return 0;
}