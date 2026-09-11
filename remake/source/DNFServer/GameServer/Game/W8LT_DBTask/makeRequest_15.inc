// DB_UpdateGMData::makeRequest —— ORIG T 0x0843982c sz 0xde 逐指令转写
// （报告缺失按 §3.2 兜底：objdump 区间 [0x0843982c,0x08439909]）：
// Acquire("DBThread.cpp",0x8761) → CStreamGuard(stream,true)
//   → <<0x14a(int) → <<-1(int) → <<flag(bool) → put(DB_Q(2))。
// 声明见 W8LT_DBTask.h（权威点）；CGmRegistEvent.cpp StartEvent 调用。
void DB_UpdateGMData::makeRequest(bool flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8761), true);
    guard.operator*()->operator<<((int)0x14a);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((bool)flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}