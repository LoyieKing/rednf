bool _isPackageItem(const CItem* item)
{
    // ORIG 双物化（内层 || 链 mov$1/mov$0 岛 + 外层 test/sete 归一）⇒
    // if(cond)return true;return false; 形（P56）。
    if (item->GetItemType() == 0xf || item->GetItemType() == 0xe ||
        item->GetItemType() == 0x10)
        return true;
    return false;
