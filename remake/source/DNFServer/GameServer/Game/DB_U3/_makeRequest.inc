Stream* DB_Insert0712WinterEventCoupon::_makeRequest(ENUM_INTERNALPACKET op,
                                                     unsigned int accId,
                                                     unsigned int characNo,
                                                     unsigned int count)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7331), false);
    guard.operator*()->operator<<((int)op);
    guard.operator*()->operator<<(-1);

    SIG_INSERT_0712_WINTER_EVENT_COUPON* v =
        guard.operator->()->GetInBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>();
    v->m_accId = accId;
    v->m_characNo = characNo;
    v->m_count = count;

    return guard.Get();
