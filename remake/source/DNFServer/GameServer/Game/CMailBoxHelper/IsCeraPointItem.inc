bool CMailBoxHelper::IsCeraPointItem(unsigned long itemIdx)
{
    // ORIG 0x08553912：0x28eba2 / 0x28ec3d 两固定 idx
    return itemIdx == 0x28eba2 || itemIdx == 0x28ec3d;
