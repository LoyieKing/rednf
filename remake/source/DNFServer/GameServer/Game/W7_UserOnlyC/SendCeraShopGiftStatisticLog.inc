void CUser::SendCeraShopGiftStatisticLog(unsigned int arg1, unsigned int arg2,
                                         unsigned long val64,
                                         unsigned int arg4)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xa2c0), true);
    // ORIG 每次写入均重调 operator*（P48），GetInBuffer 经 operator->。
    (**guard) << 0x29c;
    (**guard) << static_cast<int>(GetUID());

    SIG_STATISTIC_CERASHOP_GIFT_LOG* sig =
        guard->GetInBuffer<SIG_STATISTIC_CERASHOP_GIFT_LOG>();
    // §4.3：具名成员访问（SigTypes_E.h 已按 ORIG 偏移字段化），禁裸偏移。
    sig->m_accId = m_accId;
    sig->m_characNo = static_cast<unsigned int>(getCurCharacNo());
    sig->m_buyIdx = arg1;
    sig->m_cost = arg2;
    sig->m_itemAddInfo = static_cast<unsigned int>(val64);
    sig->m_itemAddInfo2 = arg4;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
