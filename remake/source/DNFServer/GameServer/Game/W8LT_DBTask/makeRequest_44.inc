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
    // [R12a20 ④ · §15-D 循环不变式] ORIG 在循环外对 m.end() 求值一次并缓存：
    //   ORIG 0x08454124 `call _ZNKSt3mapIhsSt4lessIhESaISt4pairIKhsEEE3endEv`
    //   位于 begin 之后、循环回边之前（函数内 end 调用共 1 次，缓存槽 -0x18）；
    //   我方原把 end() 写在循环条件内 ⇒ 每轮调用一次（.o 0x0832eb3a）。
    std::map<unsigned char, short>::const_iterator it = m.begin();
    std::map<unsigned char, short>::const_iterator itEnd = m.end();
    for (; it != itEnd; ++it)
    {
        guard.operator*()->operator<<((unsigned char)it->first);
        guard.operator*()->operator<<((short)it->second);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}