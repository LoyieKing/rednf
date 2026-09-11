// ---- send_db —— ORIG 0x0827d127 ----
// 投递 0x54 头 premium DB 流（uid/accId/type/start/end/flag/serverCode）至 DB_Q。
void premium_helper_function::send_db(CUser* user, int type, long start, long end,
                                      int flag)
{
    // ORIG 0x0827d0f5：acc_id/target_server 按无符号(lsEj)、start/end 按 long(lsEl) 写。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0x295),
                           true);
    guard.operator*()->operator<<((int)0x54);
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<((unsigned int)user->get_acc_id());
    guard.operator*()->operator<<((int)type);
    guard.operator*()->operator<<(start);
    guard.operator*()->operator<<(end);
    guard.operator*()->operator<<((int)flag);
    guard.operator*()->operator<<((unsigned int)get_target_server(type));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}