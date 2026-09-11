unsigned short CItemGloballyUniqueIdentifierGenerator::getSeq()
{
    ++m_seq;
    if (m_seq > 0xea5f)
        m_seq = 0;
    return m_seq;
}