// ---- makeRequest（InterSelectMobileAuthReward）----
// ORIG T 0x081615ae：同 CreateDnfUserInfo 模式，internal_stream(guard, 0xc,
// param_1)；空 → logFormat(0x18b)；否则写 +0x0/+0x4 入 Msg 并 put(1, guard)。
// [N1F 2026-09-11 §6.5 串盲区] ORIG .rodata `_ZZN27InterSelectMobileAuthReward11makeRequestEijbE19__PRETTY_FUNCTION__`
//   = `static void InterSelectMobileAuthReward::makeRequest(int, memberIdentificationNumber_t, bool)`
//   （strall 审计实读；我方原写 `unsigned int` ⇒ 串内容不符）。mangled `...Eijb` 不变
//   （typedef 不入 mangled）。同 typedef 先例：CGuildServerProxy.cpp:151 / DB_Login.cpp:89 /
//   CUser.cpp:780 / W8LT_DBTask/makeRequest_3.inc:1（同型重复 typedef 合法）。
typedef unsigned int memberIdentificationNumber_t;
void InterSelectMobileAuthReward::makeRequest(int param_1, memberIdentificationNumber_t param_2,
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
}