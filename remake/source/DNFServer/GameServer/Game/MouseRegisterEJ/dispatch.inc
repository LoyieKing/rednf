int DB_MouseRegister::dispatch(int param1, int param2, Stream* stream)
{
    // ORIG 栈槽：0x8=this 0xc=param1(未用) 0x10=param2 0x14=stream。
    char buf58[36];        // 明文缓冲（-0x54）
    char buf7c[36];        // encryptAES_ 密文缓冲（-0x78）
    MySQL* db;             // -0x18（仅赋值未消费，ORIG 同）
    SIG_MOUSE_REGISTER* out;   // -0x14
    int ret;               // -0x10
    SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD* p;   // -0xc

    (void)param1;
    db = _getDBHandleForDevideAccountDB();
    out = stream->GetOutBuffer<SIG_MOUSE_REGISTER>();
    ret = 0;
    char buf2c[16] = { '\0' };   // webAddress 快照（-0x28；声明置 GetOutBuffer 后，
                                 // 清零指令序与 ORIG 一致）
    strncpy(buf2c, out->m_webAddress, 0x10);
    strncpy(buf58, out->m_password2, 7);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, buf58, buf7c);
    if (strncmp(out->m_padPassword, buf7c, 0x24) == 0)
    {
        CStreamGuard guard2(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5b59), true);
        (**guard2) << 0x246;
        (**guard2) << param2;
        // ORIG 0x08422a94 实有中间槽（`mov %eax,-0xc; mov -0xc,%eax; movl
        // $0,(%eax)`）＝ 具名 p 变量形态（fn_diff_tu 实证）。
        p = guard2->GetInBuffer<SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD>();
        p->m_adviceChangePwd = 0;   // [b28d-47] 具名字段（同上）
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard2);
        return 0;
    }
    switch (out->m_pkgType)
    {
    case 1:
        ret = _registNewPassword(out->m_accId, buf7c, buf2c, param2);
        if (ret == 0)
        {
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1a = 1;
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1f = 0;
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1b = 0;
            memcpy(out->m_padPassword, buf7c, 0x24);
        }
        break;
    case 2:
        ret = _modifyPassword(out->m_accId, buf7c, buf2c, param2,
                              out->m_advideChangePwd);
        if (ret == 0)
        {
            memcpy(out->m_padPassword, buf7c, 0x24);
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1f = 0;
            reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field1b = 0;
        }
        break;
    case 3:
        ret = _unregistPassword(out->m_accId, buf2c);
        if (ret == 0)
        {
            memset(out->m_padPassword, 0, 0x24);
        }
        break;
    }
    _sendResult(out->m_accId, ret, out->m_pkgType, param2, out);
    return 1;
