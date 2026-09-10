bool CCreatureMgr::LoadArtifact(int slot, Inven_Item* item)
{
    // ORIG 0x0833be8d：`test %al,%al; je <Equip>`（isEmpty 直测不取反，空→mov$1
    // 返 1，非空→EquipArtifact）。`!isEmpty()` 被 4.4 折散 xor$1 物化 → 改正向
    // `if (isEmpty()) return true;`（P22/P194 直测不取反形）。
    if (item->isEmpty())
        return true;
    return EquipArtifact(slot, item);
