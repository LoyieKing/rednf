void CVillageMonsterArea::OnRunVillageMonster()
{
    if (G_GameWorld()->GetUserCount(m_field28, m_field2c) == 0)
        return;
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        CVillageMonster* monster = &it->second;
        monster->OnRunVillageMonster((std::vector<MapArea>*)m_areaList,
                                     m_field30);
    }
    SendVillageMonsterPosition();
    m_field30 = (ENUM_VILLAGE_MONSTER_ROTATE)(m_field30 + 1u);
