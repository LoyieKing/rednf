bool CCreatureMgr::IsCreatureEquipmentScope(int slot)
{
    // ORIG 833926c：js→cmp16(7e) / jle→mov1(84) / jne→mov0(8b)。
    // && 失败与 || 右操作数共享 cmp16 块；then 块在条件链之后。
    if ((slot >= 0 && slot <= 0x8b) || slot == 0x16)
        return true;
    return false;
}
