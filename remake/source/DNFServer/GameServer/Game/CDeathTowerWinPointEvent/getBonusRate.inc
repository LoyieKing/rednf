float CDeathTowerWinPointEvent::getBonusRate()
{
    if (IsEventing((CUser*)0))
        return (float)m_ratio / 100.0f;
    return 0.0f;
