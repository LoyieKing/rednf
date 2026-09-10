stVictoryPenalty PowerParameterScript::GetPenalty(int victory)
{
    // [b28d-solo-CDataManagerScripts] P13 反向：ORIG 0x08a59ce3 循环条件处每轮
    // 调 end()（end 临时槽 -0xc），不缓存；it 槽 -0x10。
    for (std::vector<stVictoryPenalty>::iterator it = m_vecBC.begin();
         it != m_vecBC.end(); ++it)
    {
        stVictoryPenalty entry = *it;
        if (entry.m_field4 >= victory)
            return entry;
    }
    // ORIG：无条件整体赋值（memmove 16B，无 empty 保护；size==0 时为
    // 与 ORIG 相同的越界读，勿加保护）。
    return m_vecBC[m_vecBC.size() - 1];
