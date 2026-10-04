// [R20a07 §26/P292-T3] int→bool（ORIG 调用点 `call; jmp epilogue` 无 test/setne
// 归一化 ⇒ bool callee；返回类型不入 mangled、不改布局）。声明同步于
// W8LT_DBTask.cpp / OrigDBInter19.cpp 两处 TU-local 类定义。
bool DB_ReqQueryCharacInfo::_sendResult(int errCode, int slot, char const* name,
                                        int characNo, int job, int lev,
                                        int growType, int sex, int field2e)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6716), true);

    guard.operator*()->operator<<(0xc4);
    guard.operator*()->operator<<(slot);

    SIG_QUERY_CHARAC_INFO* view =
        guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    view->m_errCode = errCode;
    if (errCode == 0)
    {
        strncpy(view->m_name, name, 0x1d);
        view->m_characNo = characNo;
        view->m_job = static_cast<unsigned char>(job);
        view->m_lev = static_cast<short>(lev);
        view->m_growType = static_cast<unsigned char>(growType);
        view->m_sex = static_cast<unsigned char>(sex);
        view->m_pad2e[0] = static_cast<unsigned char>(field2e);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}