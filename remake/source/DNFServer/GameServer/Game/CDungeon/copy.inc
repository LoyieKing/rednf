void STWarRoomData::copy(STWarRoomData* other)
{
    // [b28d-solo-CDungeon 判词 · IDENTICAL_AE（gap 0）] ORIG 0x0836a578 / our 298 = ORIG 298；
    // p167 24=24 CLEAR。本轮修法：①m_vec14/m_vec6c/m_dc 合并进同一 i 循环 ——
    //   ORIG 0x836a5bc..0x836a759 单循环含三段（旧判词「ORIG 不写 m_dc」系地址算错：
    //   ORIG `add $0x34,%edx; mov 0xc(%eax,%edx,4),%edx` = m_dc[i]，+0xdc+4i，
    //   P171 照抄恢复 `other->m_dc[i] = m_dc[i];`）；②首个 memcpy 实参改
    //   `(char*)other, (const char*)this` 复现 ORIG 先备 this→%edx/other→%eax 再压
    //   size 的装载序（P68 形；(void*) 形反劣）。残余仅首处 memcpy 装载时刻与槽号
    //（N82/P75 跨版本）。结构整体赋值 rep movsl 分块启发式差（N84/N90）不在此件。
    // ORIG 0x836a578：数组按元素 reserve/push_back，标量逐个/memcpy。
    memcpy((char*)other, (const char*)this, 0xc);
    other->m_c = m_c;
    other->m_10 = m_10;
    for (int i = 0; i < 4; ++i)
    {
        other->m_vec14[i].reserve(m_vec14[i].size());
        for (size_t j = 0; j < m_vec14[i].size(); ++j)
            other->m_vec14[i].push_back(m_vec14[i][j]);
        other->m_vec6c[i].reserve(m_vec6c[i].size());
        for (size_t j = 0; j < m_vec6c[i].size(); ++j)
            other->m_vec6c[i].push_back(m_vec6c[i][j]);
        other->m_dc[i] = m_dc[i];
    }
    other->m_vec44.reserve(m_vec44.size());
    for (size_t i = 0; i < m_vec44.size(); ++i)
        other->m_vec44.push_back(m_vec44[i]);
    other->m_50 = m_50;
    other->m_54 = m_54;
    other->m_58 = m_58;
    memcpy(other->m_5c, m_5c, sizeof(m_5c));
    memcpy(other->m_9c, m_9c, sizeof(m_9c));
    other->m_ac = m_ac;
    other->m_spawnProb.reserve(m_spawnProb.size());
    for (size_t i = 0; i < m_spawnProb.size(); ++i)
        other->m_spawnProb.push_back(m_spawnProb[i]);
    other->m_bc = m_bc;
    other->m_c0 = m_c0;
    memcpy(other->m_c4, m_c4, sizeof(m_c4));
    other->m_d4 = m_d4;
    other->m_d8 = m_d8;
    other->m_ec = m_ec;
    other->m_f0 = m_f0;
    other->m_f4 = m_f4;
    other->m_f8 = m_f8;
    other->m_fc = m_fc;
}