bool CDungeon::IsEnterEachMap() const
{
    // ORIG 0x08365278：return m_enterEachMap（movzbl 0x6d0）
    return m_enterEachMap;
}