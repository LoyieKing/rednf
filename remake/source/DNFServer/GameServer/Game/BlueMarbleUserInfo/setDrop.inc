void BlueMarbleUserInfo::setDrop(int drop)
{
    if (m_buff.m_drop * drop >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_drop = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_drop = m_buff.m_drop * drop;
    }
