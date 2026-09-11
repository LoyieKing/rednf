// ============================================================================
// get（ORIG W 0x0822b5f2 调用链）
// ============================================================================
STExpertJobScript* CExpertJobList::get(int job)
{
    std::map<int, STExpertJobScript*>::iterator it = m_jobMap.find(job);
    // b28d-61 [P222 翻正，ORIG 0x0822b3bf `je +0x5d`→mov$0 岛、命中块 fall-through]：
    // 旧「否定早返」形 `if (it == end()) return 0; return it->second;` 在 4.4 产
    // 反向块序（je→命中块、mov$0 落 fall-through，diff 8）；改正向形后 32/32 全等。
    if (it != m_jobMap.end())
        return it->second;
    return 0;
}