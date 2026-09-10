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
        // b28d-65：ORIG 叶块排放序（P46）= 源码 case 书写序 0,2,3,7,1,0x12,0x13..0x16
        // （0x044 GetInvenRef1 / 0x076 GetCargoRef / 0x08c GetInvenRef0 / 0x0b4 GetInvenRef3 /
        //  0x0e6 GetInvenRef2 / 0x101.. / 0x141..）⇒ case 1 移到 case 7 之后。
        case 2:
            return getCurCharacCargoW()->GetCargoRef(slot);    // [0x8680fa4 GetCargoRefEi]
        case 3:
            return getCurCharacInvenW()->GetInvenRef(0, slot); // [0x8680fba]
        case 7:
            return getCurCharacInvenW()->GetInvenRef(3, slot); // [0x868100a]
        case 1:
            return getCurCharacInvenW()->GetInvenRef(2, slot); // [0x8680f9a]
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
