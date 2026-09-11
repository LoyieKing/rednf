// ============================================================================
// DB_LoadCerashopAddRestrictType::dispatch @ 0x080e1aca（报告 dispatch.md 全文转写）
// 读流：m_id | characNo；账号表（handle 1）查询 limit 10，n_rows>0 时组包
// 0x341 | param2 | SIG（count + type=0 + 每行 3 uint 自 +8 步长 0xc）投 DISPATCH_Q；
// 角色表（handle 2）同构（type=1，行号 0x1da）。失败 false，成功 true。
// ============================================================================
bool DB_LoadCerashopAddRestrictType::dispatch(int /*param1*/, int param2,
                                              Stream* stream)
{
    // [b28d-46 判词·N/N82] ORIG 0x080e1aca：354 vs 358；帧 0x50 vs 0x60 与
    // 槽号整体平移（N82/P112）⇒ 封顶。
    unsigned int mId = 0;      // -0x30
    unsigned int characNo = 0; // -0x34
    *stream >> mId;
    *stream >> characNo;

    // ---- 账号表 ----
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)1, (ENUM_SERVER_GROUP)0);
    if (db == 0)
        return false;
    db->set_query(
        "seLect ipg_no, count, next_date from account_cerashop_restrict where "
        "m_id = %u and end_date > UNIX_TIMESTAMP() limit 10", mId);
    if (!db->exec(true))
        return false;
    int nRows = db->get_n_rows();   // -0x28
    if (nRows > 0) {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x1bc);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x341);   // SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
        guard.operator*()->operator<<(param2);
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig =
            guard.operator->()->GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        // [P6S6 2026-09-11 复核·仍 N] 试 `((int*)sig)[1] = 0;`（期望 disp 折叠
        //   `movl $0x0,0x4(%eax)`）实测我方仍 `add $0x4,%eax; movl $0x0,(%eax)`，
        //   读数恒 358/354 ⇒ 4.4 对 int* 索引 cast 不折叠（N82 族）⇒ 回滚原形。
        *(int*)((char*)sig + 4) = 0;      // +0x04 type = account
        for (int i = 0; i < nRows; ++i) {
            if (!db->fetch())
                return false;
            // [P6S6 2026-09-11 负结论·回滚] ORIG 0x080e1c48 段为 `col++` 形
            //   （`mov -0x18,%eax; addl $0x1,-0x18` ×3 直证），但本波实测
            //   `int col = 0; get_int(col++, …)` 反而劣化：ours 358→**366**、
            //   difflib 块 22→25（4.4 对后置自增实参多 8 条）⇒ 回滚维持常量形
            //   （语义等价：两侧都传 0/1/2 列号）。
            int col = 1;   // -0x18 列号推进（0 基）
            if (!db->get_int(0, *(unsigned int*)((char*)sig + 8 + i * 0xc)))
                return false;
            col = 2;
            if (!db->get_int(1, *(unsigned int*)((char*)sig + 0xc + i * 0xc)))
                return false;
            col = 3;
            if (!db->get_int(2, *(unsigned int*)((char*)sig + 0x10 + i * 0xc)))
                return false;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }

    // ---- 角色表 ----
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    if (db2 == 0)
        return false;
    db2->set_query(
        "seLect ipg_no, count, next_date from charac_cerashop_restrict where "
        "charac_no = %u and end_date > UNIX_TIMESTAMP() limit 10", characNo);
    if (!db2->exec(true))
        return false;
    nRows = db2->get_n_rows();
    if (nRows > 0) {
        Stream* s = GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x1da);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<(0x341);
        guard.operator*()->operator<<(param2);
        SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE* sig =
            guard.operator->()->GetInBuffer<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE>();
        *(int*)sig = nRows;               // +0x00 count
        *(int*)((char*)sig + 4) = 1;      // +0x04 type = charac
        for (int i = 0; i < nRows; ++i) {
            if (!db2->fetch())
                return false;
            int col = 1;
            if (!db2->get_int(0, *(unsigned int*)((char*)sig + 8 + i * 0xc)))
                return false;
            col = 2;
            if (!db2->get_int(1, *(unsigned int*)((char*)sig + 0xc + i * 0xc)))
                return false;
            col = 3;
            if (!db2->get_int(2, *(unsigned int*)((char*)sig + 0x10 + i * 0xc)))
                return false;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return true;
}