void CIPGHelper::IPGResult(SIPGData* req, SIPGData* res)
{
    // [b28d-solo-W6_IPGDeps] ORIG 0x0810085e：Acquire 结果无具名局部，直接作
    // CStreamGuard ctor 实参（P259）⇒ 内联实参；guard 槽回到 ORIG -0x10。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("IPGHelper.cpp", 0x196), true);

    guard.operator*()->operator<<(0x48);
    guard.operator*()->operator<<(-1);
    // [b28d-solo-W6_IPGDeps] ORIG 块序：res!=0 块直落于前、res==0 块经 je 落于后
    //（0x08100985 <+0x127>）⇒ 写 if (res != 0){B} else {A}（P47/P59 极性布局）。
    if (res != 0)
    {
        // ORIG 081008f5：载荷前 0x41 字节整段搬运（跨 m_account/m_accName/
        // m_characName/m_gameCode 与 m_cmd 首部），非单字段写。
        memcpy(&res->m_account, &req->m_account, 0x41);
        res->m_uid = req->m_uid;
        res->m_callback = req->m_callback;
        res->m_type = req->m_type;
        guard.operator*()->operator<<('\x01');
        guard->put_binary(res, 0x2dc);   // 同上（线格式整块）
        guard->put_binary(req, 0x2dc);   // 同上
    }
    else
    {
        guard.operator*()->operator<<('\0');
        guard->put_binary(req, 0x2dc);   // 整块线格式序列化（P156 豁免：协议字节流 n=sizeof(SIPGData)）
    }

    // [b28d-solo-W6_IPGDeps] ORIG 块序：msgq put 块直落于前、Free 块经 je 落于后
    //（0x081009eb <+0x18d>）⇒ 写 if (notifyFlag != 0){put} else {Free}。
    if (req->m_notifyFlag != 0)
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
    else
    {
        GlobalData::s_stream_pool->Free(guard);
    }
}
