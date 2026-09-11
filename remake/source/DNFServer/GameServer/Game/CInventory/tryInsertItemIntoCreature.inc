int CInventory::tryInsertItemIntoCreature(Inven_Item& item) const
{
    int result = -1;
    if (item.isEquipableItemType())
    {
        result = GetEmptyCreatureSlot(item);
    }
    else
    {
        result = GetExistCreatureSlot(item);
        // b28c-L2: ORIG 0x850aaef `shr $0x1f,%eax; test %al,%al; je`（4.1 负数判定
        // 经无符号右移 31 物化到 al）；4.4 对 `result < 0` 恒折 `cmpl/jns`（探针 l2_probe2
        // c2 形命中）⇒ 保留等价语义的显式无符号移位形。
        if (static_cast<unsigned char>(static_cast<unsigned int>(result) >> 31))
        {
            result = GetEmptyCreatureSlot(item);
        }
    }
    return result;
}