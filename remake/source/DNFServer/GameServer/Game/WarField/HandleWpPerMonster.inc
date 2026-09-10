int WarField::HandleWpPerMonster(CUser* user, int monsterIdx, int wp,
                                 map_monster& out)
{
    MapInfo* mi = &m_mapInfo;
    std::map<int, map_monster>::iterator it = mi->m_monsterMap.find(monsterIdx);
    if (it == mi->m_monsterMap.end())
    {
        return 0;
    }
    map_monster* mob = &it->second;
    std::map<int, stWarPoint, std::greater<int> >* warMap =
        &G_CDataManager()->m_warAreaCounter.m_wpPerMonsterHp;  // +0x87b4+0x120=0x88d4
                                                              // （ORIG 086ba266 add $0x88d4 实证）
    std::map<int, stWarPoint, std::greater<int> >::iterator it2 = warMap->find(wp);
    std::map<int, stWarPoint, std::greater<int> >::iterator it3 =
        warMap->find(mob->m_dropRate);
    if (it2 == warMap->end())
    {
        return 0;
    }
    ++it2;
    if (it3 == warMap->end())
    {
        return 0;
    }
    ++it3;
    if (!(wp < mob->m_dropRate))
    {
        return 0;
    }
    int total = 0;
    WpBonusPointResult bonus;
    user->checkBonusPoint();
    user->getWpBonusPoint(bonus);
    while (it3 != it2 && it3 != warMap->end())
    {
        total += it3->second.m_pts[mob->m_roleType];
        ++it3;
    }
    total += bonus.m_base;
    total += (int)((float)total * ((float)bonus.m_add / 100.0f) + 0.5f);
    mob->m_dropRate = wp;
    out = *mob;
    return total;
