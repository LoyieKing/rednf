void DB_HadesPunish::makeRequest(unsigned int a, unsigned int b,
                                 unsigned int c, unsigned int d,
                                 unsigned int e, char* msg)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xad59), true);
    guard.operator*()->operator<<((int)0x2b6);
    guard.operator*()->operator<<((int)-1);
    // ORIG 0x08432fe2 单槽：call GetInBuffer 后 `mov %eax,-0xc(%ebp)` 一次
    // 落栈（无 sig 中转槽双 spill 形）⇒ view cast 直落声明初始化（技巧14）。
    DB_HadesPunishView* v = reinterpret_cast<DB_HadesPunishView*>(
        guard->GetInBuffer<SIG_HADES_PUNISH>());
    v->m_mId = a;
    v->m_punishType = b;
    v->m_days = c;
    v->m_applyFlag = d;
    v->m_field10 = e;
    if (msg != 0)
        strcpy(v->m_msg, msg);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}