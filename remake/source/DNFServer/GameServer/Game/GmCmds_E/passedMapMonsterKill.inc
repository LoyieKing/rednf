void CMonsterDropTest::passedMapMonsterKill(CParty* party, DropStat& drop,
                                            GradeStat& grade)
{
    std::list<map_item> items;
    int killCnt = 100;
    std::vector<bool> flags;
    bool flagOn1 = true;                                     // ORIG -0xe/-0xd 双 bool 槽
    bool flagOn2 = true;
    flags.push_back(flagOn1);
    flags.push_back(flagOn2);

    for (unsigned int round = 0; round < static_cast<unsigned int>(m_count); round++)
    {
        for (std::vector<MapInfo>::iterator mit =
                 static_cast<std::vector<MapInfo>*>(
                     party->getPassedMapInfoList())->begin();
             mit != static_cast<std::vector<MapInfo>*>(
                        party->getPassedMapInfoList())->end();
             ++mit)
        {
            for (std::map<int, map_monster>::iterator it = mit->m_monsterMap.begin();
                 it != mit->m_monsterMap.end(); ++it)
            {
                if (it->second.m_dropCnt != 100)
                {
                    continue;
                }
                processKillMonster(party, *mit, it->second, flags, killCnt, items);
                getDungeonDropResult(items, drop, grade);
                items.clear();
            }
        }
    }
