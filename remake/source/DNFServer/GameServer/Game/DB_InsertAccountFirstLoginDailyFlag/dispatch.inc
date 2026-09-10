bool DB_InsertAccountFirstLoginDailyFlag::dispatch(int /*userIdx*/, int /*slot*/,
                                                    Stream* s)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int local_20 = 0;   // m_id
    unsigned int local_24 = 0;   // charac_no
    // P0/P171：ORIG 0x0843e93e 仅对 p1/p2 预清（movl $0），p3 无预清（直接读入）。
    int          local_28;       // 事件记录类型（0x19c）
    // b28d-52/P171：ORIG 0x0843e8f0 头有 `movl $0x0,-0x14(%ebp)` 死写入
    // （该槽此后不再被读），照抄保留。
    int          local_14 = 0;
    (void)local_14;
    *s >> local_20;
    *s >> local_24;
    *s >> local_28;

    // P0：ORIG 无 dm 槽（G_CDataManager() 结果经 %eax 直传 get_event_script_mng）。
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    // ORIG 0x0843e950 `jne <后置 return-0 块>`（else 直落失败块、body 前置）⇒
    // 正向 if/else 形（与早返形互为镜像，探针 /tmp/flagprobe/p.o g1/g3 实证）。
    EventClassify::CEventEntity* ent = 0;
    if (local_28 == 0x19c)
    {
        ent = sm->get_event_entity(local_28);
        if (ent == 0)
            return 0;
    }
    else
        return 0;

    // ORIG 0x0843e99c/0x0843ea12：两处均为 fresh 虚调用
    // `ent->get_db_table_name()`（vptr+0x20，P48 no-CSE / P282），结果直接作
    // toTString 实参，无具名引用缓存（旧版缓存导致第二次虚调用缺失）。
    db->set_query("upDate %s set charac_no=%u, occ_date=now() where m_id=%u",
                  toTString(ent->get_db_table_name()), local_24, local_20);
    if (!db->exec(true))
        return 0;

    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into %s(m_id, charac_no, occ_date) "
                      "values(%u, %u, now())",
                      toTString(ent->get_db_table_name()), local_20, local_24);
        if (!db->exec(true))
            return 0;
    }
    return 1;
