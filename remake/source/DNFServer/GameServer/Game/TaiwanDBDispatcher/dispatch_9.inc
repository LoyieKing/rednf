// ORIG 0x0815d7a8：GetOutBuffer<Taiwan::SigLoadHeroMissionData> null → throw
// "context_null"；GetDBHandle(9,0) null → throw "handle_null"；set_query
// ("seLect mission from event_hero_mission_data where server_group=%d and
// charac_no=%u", get_server_group(), sig->m_field0) exec 失败 throw
// "exec_select"；SigLoadHeroMissionData data; data.init(); data.m_field0 =
// sig->m_field0；get_n_rows()==0 → INSERT(values(%d,%u,'',now())) exec 失败
// throw "exec_insert"、memset(data+4,0,0x100)；否则 fetch 失败 throw "fetch"、
// get_binary(0, data+4, 0x100) 失败 throw "get_binary"；Acquire(file, 0x3ea)
// → CStreamGuard(true) → internal_stream(guard, 0x12, uid) →
// GetInBuffer<SigLoadHeroMissionData> null → throw "stream_alloc"；
// memcpy(out, &data, 0x104)；MsgQueueMgr::put(DB_Q, guard)。catch
// logFormat(0x3f5) 返 0。成功返 1。
// [b28d-solo-TaiwanDBDispatcher] AE（231/231）：两处修形——①SigLoadHeroMissionData
// 字段 public 化 + 改直接成员访问（§8.3；ORIG 0x0815d7a8 的 `mov -0x18(%ebp),%eax;
// mov (%eax),%eax; mov %eax,-0x124(%ebp)` 是 data.m_field0=sig->m_field0 的 disp 直存，
// 旧 cast 视图形恒 lea+LHS-first；同一改动连带 memset/get_binary 实参序归零）；
// ②`return put(DISPATCH_Q, guard)`（ORIG 0x0815daf3 `call put; mov %eax,%ebx` 尾
// epilogue 返回 ebx；旧写 put();return 1 在 put 失败时恒返 1 = 真语义 bug）。
bool DBLoadHeroMissionEventData::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigLoadHeroMissionData* sig =
            stream->GetOutBuffer<Taiwan::SigLoadHeroMissionData>();
        if (sig == 0)
            throw "context_null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        // b28d-lane6 试形：characNo 不设具名槽，各点内联重读（ORIG 无该槽，
        // P0/P183 表达式直读）。
        // ORIG 0x0815d7a8：G_CEnvironment()+get_server_group() 供 set_query
        // （返回值不检查）；0x0815d82c 插行时再调一次（两次独立调用，P167 判据）。
        // 仅 exec(1) 失败才 throw "exec_select"。
        db->set_query(
            "seLect mission from event_hero_mission_data where server_group=%d and charac_no=%u",
            G_CEnvironment()->get_server_group(),
            reinterpret_cast<SigHeroMissionDataView*>(sig)->m_field0);
        if (!db->exec(1))
            throw "exec_select";
        Taiwan::SigLoadHeroMissionData data;
        data.init();
        data.m_field0 = sig->m_field0;
        if (db->get_n_rows() == 0)
        {
            db->set_query(
                "inSert into event_hero_mission_data values(%d,%u,'',now())",
                G_CEnvironment()->get_server_group(),
                reinterpret_cast<SigHeroMissionDataView*>(sig)->m_field0);
            if (!db->exec(1))
                throw "exec_insert";
            memset(data.m_data, 0, 0x100);
        }
        else
        {
            if (!db->fetch())
                throw "fetch";
            if (!db->get_binary(0, data.m_data, 0x100))
                throw "get_binary";
        }
        // [b21] ORIG put @0x815daf3 movl $0x1=DISPATCH_Q（QueueSweep21 回扫实证）；
        // 非 DB_Q(2)。Acquire 内联进 ctor（P134）消中间槽。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire(
                "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x3ea),
            true);
        Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x12, uid);
        Taiwan::SigLoadHeroMissionData* outSig =
            guard.operator->()->GetInBuffer<Taiwan::SigLoadHeroMissionData>();
        if (outSig == 0)
            throw "stream_alloc";
        memcpy(outSig, &data, 0x104);
        // [b28d-solo-TaiwanDBDispatcher] 真语义：ORIG 0x0815daf3 `call put;
        // mov %eax,%ebx`（尾 epilogue 返回 ebx）⇒ 原码 `return put(...)`，
        // 不是 `put(); return 1;`（put 失败时 ORIG 返 0，旧写法恒返 1）。
        return GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x3f5,
            "[HeroMission] Error load hero mission. (msg:%s)", e);
        return 0;
    }
}
