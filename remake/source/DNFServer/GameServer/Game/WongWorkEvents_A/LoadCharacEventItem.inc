// ----------------------------------------------------------------------------
// EventCommon::LoadCharacEventItem —— ORIG T 0x083f9c7b (size 0x2b2)
// loadId==0 按角色取 5 条 delete_flag=0；否则按行 id 单取。
// 组包 header 0x4b + size(-1)，SIG_LOAD_EVENTITEM 装载后投递 msgq queue 1。
// [b28d-solo-WongWorkEvents_A] AE：P259 Acquire 内联实参（去 Stream* 具名
// 局部槽）+ P225 尾测 while/break + P221 cMyTrace 全表达式临时链（翻互斥块
// cMyTrace/guard 槽方向）。
// ----------------------------------------------------------------------------
void EventCommon::LoadCharacEventItem(memberIdentificationNumber_t eventId,
                                      unsigned int characNo,
                                      memberIdentificationNumber_t loadId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (loadId == 0)
    {
        db->set_query(
            "seLect id,it_id,stack_count,event_code from charac_event_items "
            "where charac_no=%d and delete_flag=0 limit %d", characNo, 5);
    }
    else
    {
        db->set_query(
            "seLect id,it_id,stack_count,event_code from charac_event_items "
            "where id=%s and charac_no=%d", NumberToString(loadId, 0), characNo);
    }

    if (!db->exec(true))
    {
        // [b28d-solo-WongWorkEvents_A] P221：ORIG cMyTrace 槽 -0x28 浅于
        // CStreamGuard 槽 -0x30；4.4 具名声明序恒反向（cMyTrace -0x30 /
        // guard -0x20）⇒ 改全表达式临时链翻互斥块槽方向。
        // [Q6HeaderTypeAxis 2026-09-11 负结论·维持手写串] 已逐参核 ORIG 0x083f9d18/28/36：
        //   prett 符号 0x8c5ddc0（size 0x7a）串 = "void WongWork::EventCommon::
        //   LoadCharacEventItem(memberIdentificationNumber_t, unsigned int,
        //   memberIdentificationNumber_t)"（**无 `static ` 前缀**）、line 0x37、level 5；
        //   fmt 0x8c35d88 = "DB_LoadInventory::GetUserInven, exec ERROR charac_no=%u"
        //   （ORIG 原样 copy-paste 遗留，勿"修正"）。
        //   ⚠ **不可换 `__PRETTY_FUNCTION__`**：EventCommon 为 class + **static 成员**
        //   ⇒ 4.4 宏输出带 `static ` 前缀（Q8 在同 TU `InsertEventItem2EventTable` 实测：
        //   宏串 0x7e vs ORIG 0x77，恰差 7 = "static "）⇒ 换宏后串 0x81 ≠ ORIG 0x7a
        //   （内容改变 = 语义轴劣化）。定义侧 typedef 拼写（.inc:1 / .cpp:663）保留仅为
        //   与 ORIG 语义一致，对 prett 无影响（本处不用宏）。
        cMyTrace("void WongWork::EventCommon::LoadCharacEventItem("
                 "memberIdentificationNumber_t, unsigned int, "
                 "memberIdentificationNumber_t)", 0x37, 5)
            ("DB_LoadInventory::GetUserInven, exec ERROR charac_no=%u", characNo);
        return;
    }

    // ORIG 0x083f9d60：if 条件内直接调用 get_n_rows()（第一次）；
    // 下方 int rowCount = get_n_rows() 为第二次调用（0x083f9e09）。忠实保留。
    if (db->get_n_rows() == 0)
        return;

    // [b28d-solo-WongWorkEvents_A] P259：ORIG 0x083f9d8f `mov %eax,0x4(%esp)`
    // 直传 Acquire 结果，无 Stream* 具名局部（具名槽令 db/cMyTrace/guard 整体
    // 下移 4）。内联实参复现。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBEventWork.inl", 0x3e), true);
    (*guard.operator*()) << 0x4b;                      // msg id（operator<< int @0x80c8c56）
    (*guard.operator*()) << -1;                        // size 占位

    SigLoadEventItemView* sig =
        reinterpret_cast<SigLoadEventItemView*>(
            guard.operator->()->GetInBuffer<SIG_LOAD_EVENTITEM>());
    sig->m_eventId = eventId;
    sig->m_characNo = characNo;

    int rowCount = db->get_n_rows();                   // 第二次调用（ORIG 0x083f9e09）
    int loaded = 0;                                    // fetch 游标
    // [b28d-solo-WongWorkEvents_A] ORIG 0x083f9e18 `movl $0,-0xc; jmp cond` +
    // body 首 `call fetch; xor $1; test; jne exit` + cond
    // `mov loaded; cmp rowCount; setl; test; jne body`（P225 setcc 形）⇒
    // 尾测 while + `if (!fetch()) break;`，非 `loaded<n && fetch()` 短路
    // （后者把 fetch 放进条件块且用 jge 直跳）。
    while (loaded < rowCount)
    {
        if (!db->fetch())
            break;
        db->get_uint(0, sig->m_entries[loaded].m_dbId);
        db->get_uint(1, sig->m_entries[loaded].m_itemId);
        db->get_uint(2, sig->m_entries[loaded].m_stackCount);
        db->get_uint(3, sig->m_entries[loaded].m_eventCode);
        ++loaded;
    }
    // +0x08 = 第二次 get_n_rows()=rowCount（ORIG `mov -0x14,%edx; mov %edx,0x8`
    // 直写本地行数，非 fetch 已装载数 loaded ── fetch 提前失败时仍写全行数，
    // 与调用方消费口径一致；勿臆改）。
    sig->m_itemCount = rowCount;

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
}