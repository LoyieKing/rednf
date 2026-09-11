// ---- DB_Insert0712WinterEventCoupon::_makeRequest ORIG @ 0x0842e6e0 ----
// stream: opcode=op | -1 | GetInBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>：
//   +0x00=accId、+0x04=characNo、+0x08=count。Get() 返回 Stream*，0=不可投递。
// ORIG Acquire 后 guard 初始 false（release 语义差异，按原样保留）。
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
}