bool CItemGenRateTable::_itemTypeVerify(int type, const CItem* item)
{
    // 报告 _itemTypeVerify.md 逐分支转写（T 0x08534906）。虚槽核对（Main 2026-09-09
    // 全库 call* 排查）：is_stackable=add $0xc（ORIG 0x08534920）、IsCreatureItem=
    // add $0x14（ORIG 0x08534961），与我方 .o 逐槽一致。
    if (item->is_stackable())
    {
        if (item->GetItemType() == 3 && type == 3)
            return true;
        if (type == 1)
            return true;
    }
    else
    {
        if (item->IsCreatureItem() && type == 4)
            return true;
        if (type == 2)
            return true;
    }
    return false;
