bool CItem::isCerashopPackageItem() const
{
    // vtable +0x0c = GetItemType()（_GetItemType 批次实证 make_item@+0x08 后继槽）。
    return GetItemType() == 0x0e || GetItemType() == 0x0f;
