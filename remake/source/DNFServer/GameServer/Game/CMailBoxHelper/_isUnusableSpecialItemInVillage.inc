// ORIG 0x085538c0：item->GetItemType()（vtable 槽 3）==0x10 或 ==0x22 时可在
// 村庄使用 → false；否则 true。ORIG 无空指针检查，与汇编一致。
bool _isUnusableSpecialItemInVillage(CItem const* item)
{
    if (!(item->GetItemType() == 0x10 || item->GetItemType() == 0x22))
        return true;
    return false;
}