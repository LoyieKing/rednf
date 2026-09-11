// ============================================================================
// DB_UpdateFatigueAttendance::makeRequest —— ORIG 0x084447d6（报告 makeRequest.md
// 全文；static）
// ============================================================================
// Acquire("DBThread.cpp",0xab72) → CStreamGuard(stream,true) → <<0x2ac(int) →
// <<-1(int) → <<param1(uint) → <<m.size()(int) → const_iterator 遍历：
// <<it->first(uchar) → <<it->second(short) → put(DB_Q(2))。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084447d6
void DB_UpdateFatigueAttendance::makeRequest(
    unsigned int param1,
    std::map<unsigned char, short, std::less<unsigned char>,
             std::allocator<std::pair<const unsigned char, short> > > const& m)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xab72), true);
    guard.operator*()->operator<<((int)0x2ac);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((int)m.size());
    for (std::map<unsigned char, short>::const_iterator it = m.begin();
         it != m.end(); ++it)
    {
        guard.operator*()->operator<<((unsigned char)it->first);
        guard.operator*()->operator<<((short)it->second);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}