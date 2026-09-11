// ----------------------------------------------------------------------------
// GetMonsterCardBindRate — ORIG W 0x0822b2d0 range [...b358]
//   it = m_monsterCardBindRateMap.find(key)；miss → out=零值、copy dword、
//   return 0；hit → copy dword(it->second)、return 1（dword 一次拷贝 = 两 word）
// ----------------------------------------------------------------------------
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
}