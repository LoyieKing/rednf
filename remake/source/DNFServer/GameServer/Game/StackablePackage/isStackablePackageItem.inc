// ORIG 0x082819c6 (0x63)：early-return 结构（P16）：item==0 → false、
// !is_stackable → false、最后 GetItemType()==0x26（sete %al 直返）。
// [b28c-11] NEAR 封顶（34/34 条）：残余仅 call/jmp 目标显示差（N7 族寄存器分配）。
bool EventProcessManager::isStackablePackageItem(int itemId) const
{
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0)
        return false;
    if (!item->is_stackable())
        return false;
    CItem* it = item;
    return it->GetItemType() == 0x26;
}