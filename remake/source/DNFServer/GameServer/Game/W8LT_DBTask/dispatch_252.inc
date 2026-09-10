bool CLocalGlobal_DB_STATICS_BOSSTOWER::dispatch(int userIdx, int slot,
                                                 Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int keyCount = 0;
    *stream >> keyCount;
    if (keyCount <= 0)
        return 0;
    struct tm tmNow;
    __GetCurrentDNFDate(tmNow);
    char date[12];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02d", tmNow.tm_year + 0x76c,
            tmNow.tm_mon + 1, tmNow.tm_mday);
    for (int i = 0; i < keyCount; ++i)
    {
        // [A04-r5] 判据②：ORIG 对 key 走**字节视图**（0x0812825e 段：
        // `movzbl -0x47(%ebp),%eax; movzbl %al,%eax` = byte1、`movzbl -0x46` = byte2、
        // `movzbl -0x45; and $0xf` = byte3&0xf、`movzbl -0x45; shr $0x4,%al` = byte3>>4、
        // `movzbl -0x48` = byte0），共 18 条 movzbl；我方 `(key>>N)&0xff` 被 4.4 折成
        // `mov;shr;and`（widthscan ext_movzbl orig=20 / our=2）⇒ 改局部匿名 union 双视角。
        union
        {
            unsigned int m_key;
            unsigned char m_byte[4];
        } key;
        key.m_key = 0;
        *stream >> key.m_key;
        STBossDungeonTowerLog* sig = stream->GetOutBuffer<STBossDungeonTowerLog>();
        // ORIG 列映射：play_cnt=+0x0 / success_cnt=+0x4 / play_time=+0xc /
        // uniq_cnt=+0x8（UPDATE 与 INSERT 同口径）。
        // ORIG 0x0812825e：每个 key 先打一条 trace（行号 0x4f9）再 UPDATE。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4f9, 0);
        trace("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
              key.m_key, (int)key.m_byte[0], (int)(key.m_byte[3] >> 4),
              (int)(key.m_byte[3] & 0xf),
              (int)key.m_byte[2], (int)key.m_byte[1], sig->stageStartCount,
              sig->stageEndCount, sig->playTimeSum, sig->pickUpItemCount);
        db->set_query(
            "upDate log_monstertower_layer_stat SET play_cnt = play_cnt + %u, success_cnt = success_cnt + %u, play_time = play_time + %u, uniq_cnt = uniq_cnt + %u WHERE occ_date = '%s' and key_id = %u",
            sig->stageStartCount, sig->stageEndCount, sig->playTimeSum,
            sig->pickUpItemCount, date, key.m_key);
        bool needInsert = !db->exec(true) || db->getAffectedRowCount() == 0;
        if (needInsert)
        {
            db->set_query(
                "inSert into log_monstertower_layer_stat(occ_date, key_id, layer, monster_diff, charac_job, charac_grow, level, play_cnt, success_cnt, play_time, uniq_cnt) values('%s', %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
                date, key.m_key, (int)key.m_byte[0], (int)(key.m_byte[3] >> 4),
                (int)(key.m_byte[3] & 0xf),
                (int)key.m_byte[2], (int)key.m_byte[1], sig->stageStartCount,
                sig->stageEndCount, sig->playTimeSum, sig->pickUpItemCount);
            db->exec(true);
        }
    }
    return 1;
