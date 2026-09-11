// ============================================================================
// DB_LoadEventLevelupSupport::dispatch —— ORIG 0x0844cd38
// >>characNo(int) → GetDBHandle(DB_HANDLE_2,0)；句柄空返 0。seLect level,
// state from event_levelup_support where charac_no=%d；exec 失败 throw
// (unsigned int)0xc277。回包流 0x350/slot + SIG_EVENT_LEVEUP_SUPPORT_INFO
// {+0 count, +4 起 ushort level/ushort state 对（步长 4）}。逐行 fetch：
// get_ushort(0/1) 失败分别 throw 0xc28c/0xc28d/0xc28e。put(DISPATCH_Q) 返
// 1。catch(unsigned int) → 日志（省略）→ 返 0。
// ============================================================================
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 0844cd38
// [b28d-solo-4] §6.2 三件套：p167 CLEAR（ORIG 36 = OUR 36，0/0）+ gap 1（211/210）
//   ⇒ 语义完成；余 N104 EH 尾块相对序（ushort 槽已修）封顶。
// [r3 负结论] 尾部 ORIG 0844cffb `jmp 0844d012`（跳过 EH landing-pad 岛 `mov
//   %edx,%ebx; mov %eax,%esi`）在 4.4 被 jump-thread 消除、且我方把 `mov
//   $0x1,%ebx` 提前到 put 之后（ORIG 在 +0x35f）⇒ our 210/ORIG 211 恒少 1 条，
//   无源形可复现 ⇒ N104 封顶。
bool DB_LoadEventLevelupSupport::dispatch(int userIdx, int slot, Stream* stream)
{
    int characNo = 0;
    *stream >> characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc26d, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 2);
        return 0;
    }
    try
    {
        db->set_query(
            "seLect level, state from event_levelup_support where charac_no=%d",
            characNo);
        // [K5W8LTDBTask W8DiagC 类型轴 ⑦] ORIG 0x0844ce17 `movl $_ZTIi,0x4(%esp)`
        // ⇒ 抛出静态类型是 **int**（我方原 `unsigned int` 发 _ZTIj）；
        // catch 同刻改 int（throw/catch 必须同型，否则异常逃逸）。
        if (!db->exec(true))
            throw (int)0xc277;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc279), true);
        guard.operator*()->operator<<(0x350);
        guard.operator*()->operator<<(slot);
        // [Lane31W8LTDBTaskView] 权威头具名直访问。
        SIG_EVENT_LEVEUP_SUPPORT_INFO* out =
            guard->GetInBuffer<SIG_EVENT_LEVEUP_SUPPORT_INFO>();
        int rowCount = db->get_n_rows();
        out->m_count = rowCount;
        // ORIG 0x0844cebc：循环前 `movw $0,-0x4a/-0x4c` 两 ushort 槽预清
        // （P198①/P232：声明在循环外，get_ushort(int,unsigned short&) 引用形参
        // 物化），循环尾 movzwl 回写成员。
        unsigned short level = 0;
        unsigned short state = 0;
        for (int i = 0; i < rowCount; ++i)
        {
            if (!db->fetch())
                throw (int)0xc28c;
            if (!db->get_ushort(0, level))
                throw (int)0xc28d;
            if (!db->get_ushort(1, state))
                throw (int)0xc28e;
            out->m_entries[i].m_level = level;
            out->m_entries[i].m_state = state;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    catch (int e)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc297, 5)(
            "[ERROR] DB_LoadEventLevelupSupport. [line:%d]", e);
        return 0;
    }
}