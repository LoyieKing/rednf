// ============================================================================
// DB_UpdateDimension::dispatch —— ORIG 0x0843230a（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_UPDATE_DIMENSION>（视图 DB_UpdateDimensionView，
// 写侧 CUserOnlyOrig.cpp UpdateDimensionInout）。+0x0d（dimension 计数有效）时
// upDate charac_dimension_inout（6 dungeon 逐字节 movsbl 符号扩展为 int，
// charac_no=%u）；exec 失败或无影响行补 inSert；inSert exec 失败
// logFormat(0x7972) 记 DIMENSION_LOG。+0x0e（blood 计数有效）时 upDate
// charac_blood_inout（dungeon1=%d）；exec 失败或无影响行补 inSert；
// inSert exec 失败 logFormat(0x797e) 记 BLOOD_LOG。恒返 1。
// ============================================================================
bool DB_UpdateDimension::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_DIMENSION* sig = /*SIG_UPDATE_DIMENSION*/(
        stream->GetOutBuffer<SIG_UPDATE_DIMENSION>());
    if (sig->m_dimensionInoutSave != 0)
    {
        db->set_query(
            "upDate charac_dimension_inout set dungeon1=%d, dungeon2=%d, dungeon3=%d,  dungeon4=%d, dungeon5=%d, dungeon6=%d where charac_no=%u",
            (int)sig->m_dungeon[0], (int)sig->m_dungeon[1], (int)sig->m_dungeon[2],
            (int)sig->m_dungeon[3], (int)sig->m_dungeon[4], (int)sig->m_dungeon[5],
            sig->m_characNo);
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into charac_dimension_inout(charac_no,dungeon1,dungeon2,dungeon3,dungeon4,dungeon5,dungeon6) values(%u,%d,%d,%d,%d,%d,%d)",
                sig->m_characNo,
                (int)sig->m_dungeon[0], (int)sig->m_dungeon[1],
                (int)sig->m_dungeon[2], (int)sig->m_dungeon[3],
                (int)sig->m_dungeon[4], (int)sig->m_dungeon[5]);
            if (db->exec(true) != 1)
            {
                LogManager::logFormat(
                    1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x7972,
                    "DIMENSION_LOG : INOUT CNT INSERT DB ERROR!! values(characno:%u,dungeon1:%d,2:%d,3:%d,4:%d,5:%d,6:%d)",
                    sig->m_characNo,
                    (int)sig->m_dungeon[0], (int)sig->m_dungeon[1],
                    (int)sig->m_dungeon[2], (int)sig->m_dungeon[3],
                    (int)sig->m_dungeon[4], (int)sig->m_dungeon[5]);
            }
        }
    }
    if (sig->m_bloodInoutSave != 0)
    {
        db->set_query(
            "upDate charac_blood_inout set dungeon1=%d where charac_no=%u",
            (int)sig->m_bloodDungeon1, sig->m_characNo);
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into charac_blood_inout(charac_no,dungeon1) values(%u,%d)",
                sig->m_characNo, (int)sig->m_bloodDungeon1);
            if (db->exec(true) != 1)
            {
                LogManager::logFormat(
                    1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x797e,
                    "BLOOD_LOG : INOUT CNT INSERT DB ERROR!! values(charac_no:%u,dungeon1:%d)",
                    sig->m_characNo, (int)sig->m_bloodDungeon1);
            }
        }
    }
    return 1;
}