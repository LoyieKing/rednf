// ORIG T 0x08680c8a（const 重载）：guard/getter 全部走 const R 版
// （getCurCharacInvenR/getCurCharacCargoR/GetInvenRefR/GetCargoRef const/
// GetCharacExpandDataR/GetInvenRefR）——ORIG 无 enableSaveInven 副作用，
// 仅 CTitleBook::getTitleSlot 直调非 const 版（_ZN…E 实证）。
const Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short area,
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
                                                  unsigned short slot) const
{
    // b28d-65：与非 const 版同修（ORIG 0x08680c8a 正极性 setne 物化 + 叶块序
    // 0,2,3,7,1,0x12,0x13..0x16 + case 0x12 具名槽往返 + default 独立 return 0）。
    if (getCurCharacR())
    {
        switch (area)
        {
        case 0:
            return getCurCharacInvenR()->GetInvenRef(1, slot);
        case 2:
            return getCurCharacCargoR()->GetCargoRef(slot);
        case 3:
            return getCurCharacInvenR()->GetInvenRef(0, slot);
        case 7:
            return getCurCharacInvenR()->GetInvenRef(3, slot);
        case 1:
            return getCurCharacInvenR()->GetInvenRef(2, slot);
        case 0x12:
        {
            CExpandEquipslot* pExpand = GetCharacExpandDataR(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
            return pExpand->GetInvenRefR(static_cast<INVEN_TYPE>(5), slot);
        }
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
            // ORIG 直调非 const getTitleSlot（_ZN…E）且用非 RE 重载
            // GetCharacExpandData(ENUM)（_ZNK5CUser19GetCharacExpandDataE23…）；
            // 经 const_cast 去 CV。
            return const_cast<CTitleBook*>(reinterpret_cast<const CTitleBook*>(
                       GetCharacExpandData(
                           static_cast<ENUM_CHARAC_EXPAND_TYPE>(14))))
                ->getTitleSlot(
                    static_cast<ENUM_TITLE_BOOK_CATEGORY>(area - 0x13), slot);
        default:
            return 0;
        }
    }
    return 0;
}