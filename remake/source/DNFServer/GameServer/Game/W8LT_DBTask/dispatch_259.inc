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
        if (!db->get_ushort(
                0,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(
                    i * 0x1b0 + (char*)out)->m_towerIndex))
            return 0;
        if (!db->get_binary(
                1,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(
                    i * 0x1b0 + (char*)out)->m_member,
                0x17))
            return 0;
        if (!db->get_uint(
                2,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x60 +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                3,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x60 +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + (char*)out)
            ->m_type = 1;
        if (!db->get_binary(
                4,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x6c +
                                                              (char*)out)
                    ->m_member,
                0x2e))
            return 0;
        if (!db->get_uint(
                5,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0xcc +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                6,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0xcc +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x6c +
                                                       (char*)out)
            ->m_type = 2;
        if (!db->get_binary(
                7,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0xd8 +
                                                              (char*)out)
                    ->m_member,
                0x45))
            return 0;
        if (!db->get_uint(
                8,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x138 +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                9,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x138 +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0xd8 +
                                                       (char*)out)
            ->m_type = 3;
        if (!db->get_binary(
                10,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x144 +
                                                              (char*)out)
                    ->m_member,
                0x5c))
            return 0;
        if (!db->get_uint(
                11,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x1a4 +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                12,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x1a4 +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x144 +
                                                       (char*)out)
            ->m_type = 4;
        ++i;
    } while (i < (unsigned int)db->get_n_rows());
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
