// ============================================================================
// DB_SaveKillMonsterInfo::dispatch —— ORIG 0x08441200（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(2,0) → GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>，空返 0；三段
// put_compressed_blob_data（field 0: sig+0x0 长 0x1d4c；field 1: sig+0x1d4c 长
// 30000；field 2: sig+0x927c 长 0x1d4c），任一段失败 cMyTrace(0xa010/0xa01a/
// 0xa024, 5)("... compress_zip(%u) ERROR charac_no=%u", 段号, charac_no) 返 0；
// upDate charac_kill_monster_info ... where charac_no=%u（charac_no=sig+0xafc8）
// exec 失败返 0；affectedRowCount==0 → inSert ...，insert exec 失败返 0；全绿 1。
// [b28d-4] DIFF：P183/P171：ORIG 多 {'movl': 1}（漏具名局部/去缓存不足）；ORIG 08441200
bool DB_SaveKillMonsterInfo::dispatch(int /*userIdx*/, int /*slot*/,
                                      Stream* stream)
{
    // [J4S2-② 回滚 2026-09-11] GetOutBuffer/空检查提到 GetDBHandle 之前实测反劣
    // （ndiff 81→87）⇒ 保持 db 先取。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        stream->GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (sig == 0)
        return 0;
    char* bossInfo = 0;    // -0x4c
    char* namedInfo = 0;   // -0x50
    char* apcBossInfo = 0; // -0x54
    bool ok = false;       // -0x9
    ok = put_compressed_blob_data(db, 0, (char*)sig, 0x1d4c, &bossInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa010, 5);
        trace("DB_SaveKillMonsterInfo::dispatch boss_info compress_zip(%u) ERROR charac_no=%u",
              0, sig->m_characNo);
        return 0;
    }
    ok = put_compressed_blob_data(db, 1, (char*)sig->m_named, 30000,
                                  &namedInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa01a, 5);
        trace("DB_SaveKillMonsterInfo::dispatch named_info compress_zip(%u) ERROR charac_no=%u",
              1, sig->m_characNo);
        return 0;
    }
    ok = put_compressed_blob_data(db, 2, (char*)sig->m_apc, 0x1d4c,
                                  &apcBossInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa024, 5);
        trace("DB_SaveKillMonsterInfo::dispatch apc_boss_info compress_zip(%u) ERROR charac_no=%u",
              2, sig->m_characNo);
        return 0;
    }
    db->set_query(
        "upDate charac_kill_monster_info set boss_info='%s', named_info='%s', apc_boss_info='%s' where charac_no=%u",
        bossInfo, namedInfo, apcBossInfo, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_kill_monster_info(charac_no, boss_info, named_info, apc_boss_info) values(%u, '%s', '%s', '%s')",
            sig->m_characNo, bossInfo, namedInfo, apcBossInfo);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}