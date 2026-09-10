void CSkillChanger::_addSPRelatedItem(int* a, int* b, int itemIdx, int count)
{
    if (itemIdx == 0x407)
    {
        *a = *a + count;
    }
    else if (itemIdx == 0x40e)
    {
        *b = *b + count;
    }
    else if (m_state.m_initialized != 0)
    {
        for (int i = 1; i <= CSkillChanger::g_nStatUpItemIdx[0]; i++)
        {
            if (CSkillChanger::g_nStatUpItemIdx[i] == itemIdx)
            {
                // b28d-D66：P183 地址重物化铁律 —— ORIG 每处读写都重发完整
                // `mov this,%eax; add $0x10,%edx/%ecx; mov 0x8/0xc(%eax,%edx,4)`
                // 两步寻址（0x0860a8cb..0x0860a901），无指针局部；原具名
                // `int* needCount = ...` 缓存使 4.4 多发 `add $0x48; mov %eax,-0x8`
                // 与 `shl/add` 序列（61 vs 56 insn）。
                m_data.m_needElixerCount[i] = m_data.m_needElixerCount[i] + count;
                m_data.m_needElixerAbility[i] = CSkillChanger::g_nElixerAbility[i];
                return;
            }
        }
    }
