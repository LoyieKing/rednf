void Arad_EventPeriodDataManager::setCreatePeriod_GM(int days)
{
    if (days < 0) goto L_skip;
    m_field8 = days;
L_skip:
    ;
