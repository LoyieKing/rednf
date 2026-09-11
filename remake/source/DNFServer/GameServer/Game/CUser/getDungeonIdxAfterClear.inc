int CUser::getDungeonIdxAfterClear()
{
    CParty* party = (CParty*)GetParty();
    if (!party)
        return -1;
    // CParty+0xb24 = BF 区内嵌视图（CParty.h m_battleField@+0xb24 建模区）。
    return party->m_battleField.get_dungeon_index(
        );
}