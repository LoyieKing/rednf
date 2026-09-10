void CUser::reqHumanCertify4ClearMap(bool flag)
{
    bool forceCertify;
    if (flag)
    {
        forceCertify = get_rand_int(0x2710) < m_mcap.getPoint();
    }
    else
    {
        forceCertify = true;
    }
    if (!forceCertify)
        return;

    // b28d-solo-CUser p167：ORIG 0x086802b8 调 getPoint 3 次（条件 `(double)getPoint()
    //   >= 100.0` 与体内 `point = getPoint()` 分开；我方缓存 point 仅 2 次）⇒ 条件直调。
    if ((double)m_mcap.getPoint() >= 100.0)
    {
        unsigned int point = m_mcap.getPoint();
        int rate = std::max(100 - (int)(point / 100), 0);
        if (rate < getStdDropRate())
        {
            {
                if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                        RestrictCategory::Enum_3, 1))
            {
                setStdDropRate(rate);
                int nowRate = getStdDropRate();
                DB_SaveUserHandicap::makeRequest(get_acc_id(), 0x9c41, 0x65,
                                                 nowRate, 1, 0);
                }
            }
        }
    }

    if (reqHumanCertify())
    {
        setHumanCertified(false);
        if (0x7ffffffe < m_humanCertifyTimerKey)
            m_humanCertifyTimerKey = 0;
        m_humanCertifyTimerKey += 1;
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                    (TIMER_MESSAGE)9, 0x3c,
                                    m_humanCertifyTimerKey, (unsigned int)-1);
    }
