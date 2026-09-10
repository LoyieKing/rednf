std::map<int, map_monster>* CBattle_Field::GetMonsterMap()
{
    return &GetCurrentMapInfo()->m_monsterMap;
