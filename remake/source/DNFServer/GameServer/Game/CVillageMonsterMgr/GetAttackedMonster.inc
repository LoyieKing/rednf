// ============================================================================
// CVillageMonsterArea::GetAttackedMonster —— ORIG T 0x86b3aea
//   返回首个 m_attackWeight >= idx 的怪物；全不满足返回最后一个；空表返回 0。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b3aea）：end() 提升到循环外
// 具名 endIt（P13 缓存 end，复现 ORIG 单次 call end）；`(*it).m_attackWeight`
// 替 `it->m` 复现 deEv（P167 it-> vs *it 判据）。p167 CLEAR。
STAttackedMonster* CVillageMonsterArea::GetAttackedMonster(int idx)
{
    if (m_monsterList.empty())
        return 0;
    std::vector<STAttackedMonster>::iterator it = m_monsterList.begin();
    std::vector<STAttackedMonster>::iterator endIt = m_monsterList.end();
    for (; it != endIt; ++it)
    {
        if ((*it).m_attackWeight >= idx)
            return &*it;
    }
    --it;
    return &*it;
}