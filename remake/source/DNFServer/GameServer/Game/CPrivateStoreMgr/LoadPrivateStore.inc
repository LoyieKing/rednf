// ORIG 0x85cab24（报告 LoadPrivateStore.md 逐指令）：申请 Stream 构造 CStreamGuard，
// 写 0x78(type)、GetUID、get_charac_no(-1) 三项后放入 DB 消息队列(DB_Q=2)。
void CPrivateStoreMgr::LoadPrivateStore(CUser* user)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xcb9), true);
    guard.operator*()->operator<<(0x78);                  // ORIG CStreamGuard::operator*
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<(user->get_charac_no(-1));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}