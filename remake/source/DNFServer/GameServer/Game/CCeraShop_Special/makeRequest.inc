// ---- DB_SaveFeaturedGoodsIdx::makeRequest —— ORIG 0x0841b2da (T) ----
// 报告 docs/class_func_reports/DB_SaveFeaturedGoodsIdx/makeRequest.md 全文已读：
// <<0x52<<-1<<param_1(accId)<<param_2(characNo)<<param_3(restrictCode)
//   <<param_5(d)<<param_4(curSec) → put(DB_Q(2))。
void DB_SaveFeaturedGoodsIdx::makeRequest(unsigned int accId, int characNo,
                                          int restrictCode, int curSec, int d)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4dfb),
                           true);
    guard.operator*()->operator<<((int)0x52);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)accId);
    guard.operator*()->operator<<((int)characNo);
    guard.operator*()->operator<<((int)restrictCode);
    guard.operator*()->operator<<((int)d);
    guard.operator*()->operator<<((int)curSec);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}