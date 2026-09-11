// ORIG 0x080f4d20。设 m_materialKind：index==0x28be7b → 2（newNPC）；在 m_materialIndices
// 中命中 → 1（fortune）；否则 0。m_materialIdx 清 0。
void Event_Upgrade_Jar::CheckMaterialIndex(unsigned long index)
{
    m_materialKind = 0;
    m_materialIdx = 0;

    if (index == 0x28be7b)
    {
        m_materialKind = 2;
        return;
    }

    // ORIG 0x080f4d20：循环比较为 (int)i < size()（Ghidra 明示），setg 有符号。
    // 原 unsigned 生 seta 反符号（P196）。
    for (int i = 0; i < (int)m_materialIndices.size(); ++i)
    {
        if (m_materialIndices[i] == index)
        {
            m_materialKind = 1;
            return;
        }
    }
}