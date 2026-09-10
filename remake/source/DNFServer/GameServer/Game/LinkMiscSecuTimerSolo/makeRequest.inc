void DB_UpdateAutoMarketConditionsControl::makeRequest(
    unsigned long long a, unsigned long long b, unsigned long long c,
    unsigned long long d, int e, int f, int g)
{
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("../RDARScript.cpp", 0x7c79), true);
        guard.operator*()->operator<<(0x15b);
        guard.operator*()->operator<<(-1);
        SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL* sig =
            guard->GetInBuffer<SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL>();
        // 具名字段（SecuLink 批次裸偏移清理；SigTypes_E.h m_values0/m_values20
        // 布局，写侧 ORIG 0x08433d70 宽度：4×8B + 3×4B）。
        sig->m_values0[0] = a;
        sig->m_values0[1] = b;
        sig->m_values0[2] = c;
        sig->m_values0[3] = d;
        sig->m_values20[0] = e;
        sig->m_values20[1] = f;
        sig->m_values20[2] = g;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
