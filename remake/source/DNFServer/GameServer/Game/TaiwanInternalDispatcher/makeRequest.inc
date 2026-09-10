void InterSelectMobileAuthReward::makeRequest(int param_1, unsigned int param_2,
                                              bool param_3)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x186),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xc), param_1);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x18b, "[Taiwan, MobileAuth] m_id:%d rewarded:%d", param_2,
            // [b28c-32] ORIG 0x08161626 cmpb/je/mov$1/mov$0 三目物化（P47）；
            // (unsigned int)param_3 直转会退化为 movzbl 单条。
            param_3 ? 1 : 0);
    }
    else
    {
        msg->m_id = param_2;
        // ORIG 0x08161218 为 4.1 bool→int 分支物化（mov$1/mov$0）；4.4 对 bool
        // 恒 movzbl 直读不可翻（P168a ③b 族），语义等价标 N。
        msg->m_rewarded = param_3 != 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
