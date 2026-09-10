unsigned int CLuckPoint::_GetMaxLuckPointUserIndex()
{
    int maxPoint = 0;
    unsigned int best = 0;
    for (unsigned int i = 0; i < m_users.size(); ++i)
    {
        if (maxPoint < m_users[i]->GetCurCharacLuckPoint())       // [0x855098d setg]
        {
            maxPoint = m_users[i]->GetCurCharacLuckPoint();       // [0x85509ae 双 call P48]
            best = i;
        }
    }
    return best;
