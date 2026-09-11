short CGameManager::getNextUID()
{
    m_uid++;  // ORIG movzwl+lea 0x1(%eax),%edx+mov %dx 单装载形态（探针 t5）
    unsigned short v = m_uid;
    if (m_uid == 0x7fff || m_uid < 0)
        m_uid = 0;
    return (short)v;
}