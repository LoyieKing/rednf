bool MySQL::exec(bool bLog)
{
    // b28d-lane11 判词 N7（本 TU 447 路由）：残余仅 3 处间接 call 目标寄存器
    // 差——ORIG 0x083f433f/0x083f437c/0x083f43da `mov (%eax),%edx … call *%edx`，
    // 4.4.7 恒落 %eax（P230 补：该形唯 4.4.6 后端可翻，本 TU 非 446h46 路由）。
    // m_nFields int→unsigned 后 +0x18 store 已回 LHS-first（P248 档③）。
    int ret;
    CDnFTimer* timer = CDnFTimerFactory::CreateDnFTimer();
    timer->SetLastTime();
    for (int retry = 0; retry < 5; retry = retry + 1)
    {
        ret = exec_query();
        if (ret == 1)
        {
            if (m_bQueryCounterEnable)
            {
                m_pQueryCounter->SetResponseTime(timer->GetTimeInterval());
            }
            CDnFTimerFactory::DestroyDnFTimer(timer);
            return false;
        }
        if (ret == 0)
        {
            break;
        }
    }
    if (m_bQueryCounterEnable)
    {
        m_pQueryCounter->SetResponseTime(timer->GetTimeInterval());
        CDnFTimerFactory::DestroyDnFTimer(timer);
    }
    if (ret == 0)
    {
        m_result = mysql_store_result(m_dbHandle);
        if (m_result != 0)
        {
            m_nRows = (int)mysql_num_rows(m_result);
            m_nFields = (int)mysql_num_fields(m_result);
        }
        else
        {
            m_nRows = 0;
            m_nFields = 0;
        }
        return true;
    }
    if (bLog)
    {
        LogManager::logFormat(1, "DBMgr.cpp", __PRETTY_FUNCTION__, 0x244,
                              "Database query error. The last query('%s') has been lost.(%d)",
                              m_query, m_dbError);
    }
    return false;
