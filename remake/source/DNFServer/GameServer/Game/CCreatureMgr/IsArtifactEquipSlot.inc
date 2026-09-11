// ORIG _ZN13user_creature12CCreatureMgr19IsArtifactEquipSlotEi T 0x083393fc：
// 槽 0x17/0x18/0x19 为神器装备槽。
bool CCreatureMgr::IsArtifactEquipSlot(int slot)
{
    return slot == 0x17 || slot == 0x18 || slot == 0x19;
}
