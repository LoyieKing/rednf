// ---- DB_AccountHack_CheckMyAccount::makeRequest ORIG @ 0x0843a13e ----
// stream: opcode 0x18b | param_1(uint)
// in-buffer(SIG_CHECK_MY_ACCOUNT_IN_DB): +0x00=param_3, +0x04=param_2,
//   +0x08 memset 0x1d 后 strncpy(param_5, param_6), +0x25=param_4
// [b28c-33 判词 N104] ORIG 正常路径尾块居末、unwind 块居中（81 条）；4.4 就地内联
// epilogue、unwind 压最末且正常路径多 1 条 `mov $0x0,%ebx`（EH 状态初始化，82 条）。
// 块内容逐条全等，仅两尾块互换 ⇒ N104（按 §6.2 三件套验收，p167 待核）。
void DB_AccountHack_CheckMyAccount::makeRequest(unsigned int uid,
                                                unsigned int accId,
                                                unsigned int a,
                                                unsigned int money,
                                                char* senderName, int nameLen)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8846), true);
    guard.operator*()->operator<<(0x18b);  // SIG_CHECK_MY_ACCOUNT
    guard.operator*()->operator<<((int)uid);

    SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
        guard->GetInBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
    sig->m_param0 = a;
    sig->m_accId = accId;
    sig->m_money = money;
    std::memset(sig->m_name, 0, 0x1d);
    std::strncpy(sig->m_name, senderName, nameLen);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}