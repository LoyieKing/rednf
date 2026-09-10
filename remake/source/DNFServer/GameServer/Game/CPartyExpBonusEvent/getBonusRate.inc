float CPartyExpBonusEvent::getBonusRate(int memberCount, bool isHelpAbuse)
{
    // P0：ORIG 虚调结果直 test，无 bool 槽（右值内联）
    if (IsEventing((CUser*)0) && memberCount > 1 && !isHelpAbuse)
        return (float)m_field0c / 100.0f;
    return 0.0f;
