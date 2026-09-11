int CInventory::insertItemIntoCreature(Inven_Item& item, eItemAddReason reason, bool bLog, bool b)
{
    // ORIG 0x0850a86a：单出口 result 变量形（三分支 call→mov %eax,-0xc→jmp 尾
    // reload；error 路径 `mov $-1; jmp` 跳过 reload）。gap 6→0，AE 级闭合（残差
    // 仅为 .o 切片 call/jmp 目标渲染，助记符序列全等）。
    int result = 0;
    if (item.m_field1 == 7)
    {
        result = insertItemIntoCreatureStackable(item, reason, bLog);
    }
    else if (item.m_field1 == 5)
    {
        result = insertItemIntoCreatureEquipment(item, reason, bLog);
    }
    else if (item.m_field1 == 6)
    {
        result = insertItemIntoArtifactEquipment(item, reason, bLog);
    }
    else
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x2549, "History ERROR, unknown inven_Item type, insertItemIntoCreature ");
        return -1;
    }
    return result;
}