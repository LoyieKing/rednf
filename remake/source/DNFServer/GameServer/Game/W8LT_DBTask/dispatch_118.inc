bool DB_CrashDownReport::dispatch(int userIdx, int slot, Stream* stream)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmVal;
    // ORIG 六个 map 迭代器在函数顶部声明（帧 -0xb8..-0xa4，Ghidra
    // local_bc..local_a8），循环内再赋值 begin/end。P50 初始化式改法实测
    // 劣化（383→488 行）⇒ 维持先声明后赋值（C1Ev 差集 +6 记 N）。
    std::map<int, int>::iterator it;
    std::map<int, int>::iterator end;
    std::map<int, int>::iterator it2;
    std::map<int, int>::iterator end2;
    std::map<int, int>::iterator it3;
    std::map<int, int>::iterator end3;
    struct tm* ptm = localtime_r(&cur, &tmVal);
    ptm->tm_sec = 0;
    time_t dayStart = mktime(ptm);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = G_GameWorld()->getStatisticsToObserveCrash()
                    ->getAbnormalDownUserCount();
    db->set_query(
        "upDate client_down set occ_count = occ_count+%d where occ_date = cast(now() as date)",
        count);
    char r = db->exec(true);
    if (!r)
    {
        // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0x4d5c, 5)("upDate client_down failed");
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4d5f, 0)("abnormal down user : %d", count);

    G_GameWorld()->getStatisticsToObserveCrash()->lock();
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                   E_SERVER_GROUP_NONE);
    int dungeon = 0;
    int down = 0;
    it = G_GameWorld()->getStatisticsToObserveCrash()
             ->m_userLastCrashDungeonId.begin();
    end = G_GameWorld()->getStatisticsToObserveCrash()
              ->m_userLastCrashDungeonId.end();
    while (it != end)
    {
        // [b28d-solo] P167：ORIG 用 operator*（deEv）×6，我方 it-> 为 ptEv。
        dungeon = (*it).first;
        down = (*it).second;
        db2->set_query(
            "inSert into crash_down_by_dungeon(occ_time,dungeon_index,down_count) values(from_unixtime(%d),%d,%d)",
            (int)dayStart, dungeon, down);
        char c1 = db2->exec(true);
        if (!c1)
        {
            db2->set_query(
                "upDate crash_down_by_dungeon set down_count = down_count + %d where occ_time = from_unixtime(%d) and dungeon_index = %d",
                (int)dayStart, down, dungeon);
            char c2 = db2->exec(true);
            if (!c2)
                goto skip_log_dungeon;
        }
        {
            // [FL3W8LT 2026-09-10 A/B 回滚] 三处 trace3/4/5 试 P328⑩ 单语句临时链：
            // gap 仍 −22（orig 374 / our 396）、TU 四数零变（plus 314→311 /
            // minus 292→289 仅少 3 对语句行）⇒ 非本件残差来源，回滚保原两语句形。
            cMyTrace trace3(__PRETTY_FUNCTION__, 0x4d7d, 0);
            trace3("abnormal down user by dungeon : %d DUNGEON %d", dungeon,
                   down);
        }
    skip_log_dungeon:
        ++it;
    }

    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                   E_SERVER_GROUP_NONE);
    int packet = 0;
    int down2 = 0;
    it2 = G_GameWorld()->getStatisticsToObserveCrash()
              ->m_userLastCrashPacketId.begin();
    end2 = G_GameWorld()->getStatisticsToObserveCrash()
               ->m_userLastCrashPacketId.end();
    while (it2 != end2)
    {
        packet = (*it2).first;
        down2 = (*it2).second;
        db3->set_query(
            "inSert into crash_down_by_packet(occ_date,packet_index,down_count) values(cast(now() as date),%d,%d)",
            packet, down2);
        char c3 = db3->exec(true);
        if (!c3)
        {
            db3->set_query(
                "upDate crash_down_by_packet set down_count = down_count + %d where occ_date = cast(now() as date) and packet_index = %d",
                packet, down2);
            char c4 = db3->exec(true);
            if (!c4)
                goto skip_log_packet;
        }
        {
            cMyTrace trace4(__PRETTY_FUNCTION__, 0x4d98, 0);
            trace4("abnormal down user by packet : %d PACKET %d", packet,
                   down2);
        }
    skip_log_packet:
        ++it2;
    }

    MySQL* db4 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                   E_SERVER_GROUP_NONE);
    int play = 0;
    int down3 = 0;
    it3 = G_GameWorld()->getStatisticsToObserveCrash()
              ->m_userLastCrashPlayStatus.begin();
    end3 = G_GameWorld()->getStatisticsToObserveCrash()
               ->m_userLastCrashPlayStatus.end();
    // ORIG 0x0841b0b0：`jmp <底部条件>` 纯 while 形（无顶部 == 检查；
    // 我方旧 do{ if(it3==end3)break; } 多 1 处 operator== 调用点，P167 EXTRA）。
    while (it3 != end3)
    {
        play = (*it3).first;
        down3 = (*it3).second;
        db4->set_query(
            "inSert into crash_down_by_playstatus(occ_date,play_status,down_count) values(cast(now() as date),%d,%d)",
            play, down3);
        char c5 = db4->exec(true);
        if (!c5)
        {
            db4->set_query(
                "upDate crash_down_by_playstatus set down_count = down_count + %d where occ_date = cast(now() as date) and play_status = %d",
                play, down3);
            char c6 = db4->exec(true);
            if (!c6)
                goto skip_log_play;
        }
        {
            cMyTrace trace5(__PRETTY_FUNCTION__, 0x4db2, 0);
            trace5("abnormal down user by playerstatus : %d PLAYERSTATUS %d",
                   play, down3);
        }
    skip_log_play:
        ++it3;
    }
    G_GameWorld()->getStatisticsToObserveCrash()->unlock();
    G_GameWorld()->getStatisticsToObserveCrash()->reset();
    return 1;
