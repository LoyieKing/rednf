bool Secu_AccountHacking::isAvatar(unsigned int itemIdx)
{
    // ORIG T 0x08278aec（isAvatar.md）：find_item 命中且虚槽+0x10
    // IsAvatarItem（CItem vtable 第 4 虚，P5 实证）返回真。
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item != 0 && item->IsAvatarItem())
        return true;
    return false;
}