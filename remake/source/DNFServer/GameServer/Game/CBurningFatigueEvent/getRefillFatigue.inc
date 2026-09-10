unsigned short CBurningFatigueEvent::getRefillFatigue(unsigned short level,
                                                      unsigned short cur) const
{
    // ORIG 0x810a166：找第一个 (level >= m_level) 且 m_level > (level-cur)
    // （16 位值提升为 int 相减）的项，返回其 m_refillFatigue(+0x08)；越界返回 0。
    // ORIG 汇编为 || 短路求值后 test/je 判定：ja/jle 落在置 0 分支。
    for (size_t i = 1; i < m_params->size(); ++i)
    {
        if (!(level < (*m_params)[i].m_level ||
              (int)(*m_params)[i].m_level <= (int)(level - cur)))
            return (*m_params)[i].m_refillFatigue;
    }
    return 0;
