void DB_MouseRegister::_sendResult(unsigned int mid, int param2, int param3,
                                   int param4, SIG_MOUSE_REGISTER* param5)
{
    // ORIG 栈槽：0x8=this 0xc=mid 0x10=param2 0x14=param3 0x18=param4
    // 0x1c=param5（cdecl：this 入栈）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5e4e), true);
    SIG_MOUSE_REGISTER* out;   // GetInBuffer 结果（-0xc）

    (**guard) << 0xa3;
    (**guard) << param4;
    out = guard->GetInBuffer<SIG_MOUSE_REGISTER>();
    memcpy(out, param5, 0x59);
    reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_accId = mid;
    reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_pkgType = param3;
    reinterpret_cast<SIG_MR_WRITEVIEW*>(out)->m_field16 = param2;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}