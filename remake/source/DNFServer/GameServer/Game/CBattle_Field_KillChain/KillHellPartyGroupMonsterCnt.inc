void CBattle_Field::KillHellPartyGroupMonsterCnt(MapInfo& mapInfo,
                                                 map_monster& mob,
                                                 bool& killedAll)
{
    if (m_killedAllHellGroups != 0)
        return;

    // P0：ORIG 无 groupIdx 槽（logFormat 参数处 movzwl 0x336 直读重读），
    // 表达式直写两处内联。
    std::map<int, int>::iterator it =
        mapInfo.m_hellPartyGroup.find(
            static_cast<int>(mob.m_hellPartyGroupOrder));
    if (it == mapInfo.m_hellPartyGroup.end())
    {
        LogManager::logFormat(
            1, "battle_field.cpp", s_bfkc_pretty_kill_hell, 0x1dd9,
            "HELLPARTY_LOG : HELLPARTY GROUP INDEX (%d), MAP INDEX (%d) : "
            "KILL HELL GROUP ERROR!!\n",
            static_cast<int>(mob.m_hellPartyGroupOrder), mapInfo.m_mapIndex);
        return;
    }

    mob.m_roleType = 0;
    if (it->second == 0)
    {
        killedAll = true;
        mapInfo.m_hellPartyGroup.erase(it);
        if (mapInfo.m_hellPartyGroup.size() == 0)
            m_killedAllHellGroups = 1;
    }
    else
    {
        it->second -= 1;
        killedAll = false;
    }
