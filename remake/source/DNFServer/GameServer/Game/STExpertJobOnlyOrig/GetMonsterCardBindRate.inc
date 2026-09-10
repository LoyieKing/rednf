bool STExpertJobScript::GetMonsterCardBindRate(int key,
                                               stMonsterCardBindRate& out)
{
    std::map<int, stMonsterCardBindRate>::iterator it =
        m_monsterCardBindRateMap.find(key);
    if (it == m_monsterCardBindRateMap.end())
    {
        stMonsterCardBindRate zeroed;
        out = zeroed;   // ORIG miss：C1→dword 槽拷 out→return 0
        return false;
    }
    out = it->second;   // ORIG hit：pt→mov 0x4(%eax)→store out
    return true;
