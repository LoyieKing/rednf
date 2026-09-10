int DB_MouseRegister::_modifyPassword(unsigned int mid, const char* pwd,
                                      char* ip, int param4, bool param5)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=pwd 0x14=ip 0x18=param4 0x1c=param5。
    MySQL* db;      // -0x14
    MySQL* grade;   // -0x10
    SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD* p;   // -0xc

    db = _getDBHandleForDevideAccountDB();
    if (!_updateHistory(mid, 9, ip))
    {
        return 0x15;
    }
    db->set_query(
        "upDate member_mousepass set mousepass='%s',occ_time=now() where m_id=%s"
        " and enable_flag = '1'",
        pwd, NumberToString(mid, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        return 0x15;
    }
    grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_pass_mod=now() where m_id=%s",
        NumberToString(mid, 0));
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_pass_mod)"
            " values(%s, now())",
            NumberToString(mid, 0));
        if (!grade->exec(true))
        {
            return 0;
        }
    }
    if (param5)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5dcd), true);
        (**guard) << 0x245;
        (**guard) << param4;
        // ORIG 0x0842373b 实有中间槽（`mov %eax,-0xc; movzbl -0x2c,%edx;
        // mov -0xc,%eax; mov %edx,(%eax)`）＝ 具名 p 变量形态。
        p = guard->GetInBuffer<SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD>();
        // [b28d-47] 具名成员访问：4.4 对 `p->m = b` 产 ORIG 的 RHS 值先装载序
        // （探针 f3；reinterpret_cast 视图形恒 LHS 指针先）。
        p->m_adviceChangePwd = param5;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 0;
