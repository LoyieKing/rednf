void CVillageMonsterArea::OnDestroyVillageMonster()
{
    m_monsters.clear();
    SendVillageMonsterList(0);
