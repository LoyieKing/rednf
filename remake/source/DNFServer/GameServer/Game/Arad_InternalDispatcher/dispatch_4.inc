// ORIG 0x08185d20：select category, code, state where end_time >= now() →
//   SigServerState（m_flag/m_count + tagData[20]，逐行填 m_category/m_code/
//   get_binary(msg,0xc)）→ make_internal_stream_jpn(0x29, -1) + put_binary(0x195)
//   → DISPATCH_Q，成功返回 true。
// b28d-solo-Arad_InternalDispatcher: AE（clean 口径）——ORIG 0x08187024，224/224 条
//   助记符逐条全等，残差仅 .o 未解析重定位伪差；p167 调用点差集 CLEAR；
//   P0 已清寄存器绑定（N96；保留纯 register 存储类提示，P231 合规）。
bool Arad_DB_kLoadServerState::dispatch(int code, int uid, Stream* stream)
{
    // ORIG 0x08187024：GetOutBuffer<arad::SigServerState> 模板实例。
    //   %ebx 缓存返回（4.1 EH 形）。
    register bool ret;
    arad::SigServerState* sig = stream->GetOutBuffer<arad::SigServerState>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x81e, "SigServerState is null.");
        ret = false;
        goto done;
    }
    (void)sig;   // 取出仅作非空校验（ORIG 同样不再读该缓冲字段）

    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";

        if (!db->set_query(
                " seLect category, code, state from server_state_info where end_time >= now()"))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";

        // ORIG 0x08185e53：out 在 exec 后构造；循环索引 = out.m_count 本身
        //   （0x08185e66 `mov -0x1b0(%ebp),%edx` = out+1 dword，无独立 i 槽）；
        //   m_count++ 尾置 0x08185fa0。
        arad::SigServerState out;
        while (db->fetch())
        {
            // ORIG 0x08185e6c/0x08185ee2：get_int 目标 = lea out+idx*0x14(+4) 直填
            //   字段（无具名中间槽，技巧37）。
            if (!db->get_int(0, *(int*)&(out.m_data[out.m_count].m_category)))
                throw "category";
            if (!db->get_int(1, *(int*)&(out.m_data[out.m_count].m_code)))
                throw "category";
            if (!db->get_binary(2, &out.m_data[out.m_count].m_msg, 0xc))
                throw "state";
            ++out.m_count;
        }

        // ORIG 0x08185fda：Acquire 结果 %eax 直落 guard ctor 实参（无 acquired 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_DatabaseDispatcher.cpp", 0x83f), true);

        make_internal_stream_jpn(guard, (ENUM_INTERNALPACKET_JPN)0x29, -1);
        // ORIG 0x08186017/0x08186031：`call CStreamGuard::operator->` 后再
        //   `call CStreamGuard::put_binary`（p167 差集：ORIG 无 operator*）⇒
        //   源形为 `guard->put_binary(...)`（P167 判据）。
        guard->put_binary(&out, 0x195);

        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    catch (const char* e)
    {
        LogManager::logFormat(1, "localjapan/Arad_DatabaseDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x846,
                              "[SigRewardEventItem] DB_DSP Error,  (error:%s)", e);
        ret = false;
        return ret;
    }
    // ORIG 0x08186069/0x08186074：guard dtor 后 fall 入 `mov$1,%ebx` → merge
    //   ⇒ 成功赋值在 try-catch 之后（E 形，技巧23 系）。
    ret = true;
done:
    return ret;
}