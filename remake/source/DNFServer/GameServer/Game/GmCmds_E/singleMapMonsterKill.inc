// ORIG singleMapMonsterKill 0x084ad93a：当前地图全部 m_dropCnt==100 怪 × count 次。
void CMonsterDropTest::singleMapMonsterKill(CParty* party, DropStat& drop,
                                            GradeStat& grade)
{
    std::list<map_item> items;
    int killCnt = 100;
    std::vector<bool> flags;
    bool flagOn1 = true;                                     // ORIG -0x12/-0x11 双 bool 槽
    bool flagOn2 = true;
    flags.push_back(flagOn1);
    flags.push_back(flagOn2);

    MapInfo* cur = party->m_battleField.GetCurrentMapInfo();

    for (unsigned int round = 0; round < static_cast<unsigned int>(m_count); round++)
    {
        for (std::map<int, map_monster>::iterator it = cur->m_monsterMap.begin();
             it != cur->m_monsterMap.end(); ++it)
        {
            if (it->second.m_dropCnt != 100)
            {
                continue;
            }
            processKillMonster(party, *cur, it->second, flags, killCnt, items);
            getDungeonDropResult(items, drop, grade);
            items.clear();
        }
    }
}