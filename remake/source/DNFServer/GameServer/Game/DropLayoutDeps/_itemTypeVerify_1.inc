bool CSecialIMonsterDropItemGenRateTable::_itemTypeVerify(int type, const CItem* item)
{
    // 报告 WongWork__CSecialIMonsterDropItemGenRateTable/_itemTypeVerify.md：
    // 与 CItemGenRateTable 版逐分支一致。虚槽核对：add $0xc（ORIG 0x085352c0）/
    // add $0x14（ORIG 0x08535301），与我方 .o 一致。
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
}