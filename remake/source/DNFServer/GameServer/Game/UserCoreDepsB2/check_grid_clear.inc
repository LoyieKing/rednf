bool CBattle_Field::check_grid_clear()
{
    map_monster mob;
    for (std::map<int, map_monster>::iterator it =
             GetCurrentMapInfo()->m_monsterMap.begin();
         it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
    {
        mob = it->second;                             // operator= 拷贝
        if (mob.m_dropCnt == 0x64 && mob.m_field13 == 0)
            return false;
    }
    return true;
