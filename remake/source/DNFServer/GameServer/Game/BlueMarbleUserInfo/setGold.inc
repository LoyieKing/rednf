void BlueMarbleUserInfo::setGold(int gold)
{
    if (m_buff.m_gold * gold >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_gold = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_gold = m_buff.m_gold * gold;
    }
