// ============================================================================
// DB_GetDeathTowerBestRecord::dispatch —— ORIG 0x08429ad0
// 先调基类 DBDispatcher::dispatch(userIdx, slot, stream)，失败返 0。→
// >>characNo(u32) → GetDBHandle(DB_HANDLE_2,0) → seLect tower_index +
// 4×member_info/stage/play_time from charac_tower_record where charac_no=%d
// limit 5；exec 失败或无行返 0。回包流 0xe2/slot + SIG_DEATH_TOWER_GET_
// BEST_RECORD{+0 count=get_n_rows, +4 characNo}。逐行 fetch：tower_index/
// member_info_i/stage_i/play_time_i 写入行视图（行步长 0x1b0，type 标记
// 1..4）。任一步失败返 0；成功 put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 458 残差为 imul/add 地址重物化(P183) + 帧布局，见报告
// dispatch.md）
// ============================================================================
// [b28d-4] DIFF：S 档 gap=43（缺失转写/调用）+ N82 槽序；ORIG 08429ad0
bool DB_GetDeathTowerBestRecord::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // ORIG 0x08429b09 直接 `lea -0x18(%ebp),%eax; call Stream::operator>>`，
    // **无零初始化**（值完全由 >> 写入）。旧形多发 1 条 `movl $0x0,-0x18(%ebp)`
    // ⇒ 其后每条指令整体错位一行 ⇒ 356 行函数按位 diff 全线翻红（这就是本 TU
    // 大件 ndiff 被推到 300~900 的主因：1 条多余指令 × 全帧位移）。
    unsigned int characNo;
    *stream >> characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect tower_index, \t\tmember_info_1,stage_1,play_time_1, \t\tmember_info_2,stage_2,play_time_2, \t\tmember_info_3,stage_3,play_time_3, \t\tmember_info_4,stage_4,play_time_4 \t\tfrom charac_tower_record where charac_no=%d limit %d",
        characNo, 5);
    if (!(db->exec(true) && db->get_n_rows() != 0))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6bb2), true);
    guard.operator*()->operator<<(0xe2);
    guard.operator*()->operator<<(slot);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast）。
    SIG_DEATH_TOWER_GET_BEST_RECORD* out =
        guard.operator->()->GetInBuffer<SIG_DEATH_TOWER_GET_BEST_RECORD>();
    out->m_count = db->get_n_rows();
    out->m_characNo = characNo;
    // [b28d-BigW8LT 循环形复原] ORIG 0x08429c38 `movl $0x0,-0xc(%ebp)` +
    // `jmp 0x08429fdc`（先跳尾测）→ 循环体 0x08429c44，尾部
    // `addl $1,-0xc; call get_n_rows; cmp -0xc,%eax; seta; test; jne 0x08429c44`
    // = 底测 do-while 形（前置 for 头测多一次 get_n_rows 且整块错位）。
    // [G4W8LTDBTask 2026-09-10 负结论·指针加法求值序]
    // ORIG 0x08429c60 段每站点为 `imul $0x1b0,%eax,%eax; add $0x60,%eax;
    // add -0x10(%ebp),%eax; add $0xc,%eax`（base 作第二操作数、内存直加，
    // 零额外装载）；我方 4.4 恒 base-first `mov -0x10(%ebp),%eax;
    // mov -0xc(%ebp),%edx; imul; add $0x60,%edx; add %edx,%eax` ⇒ 每站点
    // 多 1 条 base 装载（全函数净 +13，orig 356 / our 370）。
    // 两种分组（`i*0x1b0+0x60+(char*)out` 与 `(char*)out+i*0x1b0+0x60`）
    // 实测编译产物逐条相同（net_load 均 13、ours 均 370）⇒ 4.4 对 ptr+int
    // 固定 base-first，P269 分组旋钮不适用，判 N（跨版本求值序）。
    unsigned int i = 0;
    do
    {
        if (!db->fetch())
            return 0;
        // [REAL-BUG 修复 + 寻址复原] 旧形 ① 行基址多算 0xc（包头已在行视图
        // m_pad0 内计入）⇒ 17 个字段整体后移 12B，末行 play_time4 写到 +0x1c0
        // 越出包体；② 提升出 `row` 行指针局部，ORIG 无此局部——ORIG 每站点
        // 都重算 `i*0x1b0 + 组偏移 + out`（0x08429c60/…c97/…cd6/…d10/…e32/
        // …e73/…eeb/…f49/…f85/…fbe），我方提升后少发 16 条 imul + 36 条
        // add/lea（实测助记符 census imul 17/1、add 41/16），并整体挪帧。
        // 故改为逐站点内联两级视图（entry 族 @行+0x00、play 族 @行+0x60，
        // 组步长各 0x6c；两族字段互叠，与 ORIG 位移逐条一致）。
        SIG_DEATH_TOWER_BEST_ROW& row = out->m_rows[i];
        if (!db->get_ushort(0, row.m_towerIndex))
            return 0;
        if (!db->get_binary(1, row.m_memberInfo1, 0x17))
            return 0;
        if (!db->get_uint(2, row.m_stage1))
            return 0;
        if (!db->get_uint(3, row.m_playTime1))
            return 0;
        row.m_type = 1;
        if (!db->get_binary(4, row.m_memberInfo2, 0x2e))
            return 0;
        if (!db->get_uint(5, row.m_stage2))
            return 0;
        if (!db->get_uint(6, row.m_playTime2))
            return 0;
        row.m_type2 = 2;
        if (!db->get_binary(7, row.m_memberInfo3, 0x45))
            return 0;
        if (!db->get_uint(8, row.m_stage3))
            return 0;
        if (!db->get_uint(9, row.m_playTime3))
            return 0;
        row.m_type3 = 3;
        if (!db->get_binary(10, row.m_memberInfo4, 0x5c))
            return 0;
        unsigned int& stage4 =
            (i + 1 < 5) ? out->m_rows[i + 1].m_prevStage4 : out->m_tailStage4;
        unsigned int& play4 =
            (i + 1 < 5) ? out->m_rows[i + 1].m_prevPlayTime4 : out->m_tailPlayTime4;
        if (!db->get_uint(11, stage4))
            return 0;
        if (!db->get_uint(12, play4))
            return 0;
        row.m_type4 = 4;
        ++i;
    } while (i < (unsigned int)db->get_n_rows());
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}