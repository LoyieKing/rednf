// ============================================================================
// DB_SaveAchievement::dispatch —— ORIG 0x0840deaa（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<stAchievement_DBSaveData> → 压缩 sig+0 起 0x1800
// 字节（m_data 段）至 buf（0x1ce7 清 0，P4 形态）；compress 失败返 0 →
// upDate charac_achievement（achievement='%s', last_update_time=now()，
// charac_no=%u）exec 失败返 0；affected==0 补 inSert（charac_no, achievement,
// last_update_time values(%u, '%s', now())）；inSert exec 失败返 0；全绿返 1。
bool DB_SaveAchievement::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    stAchievement_DBSaveData* sig =
        stream->GetOutBuffer<stAchievement_DBSaveData>();
    char buf[0x1ce7] = { 0 };
    unsigned int bufLen = 0x1ce7;
    if (!compress_zip(buf, bufLen, reinterpret_cast<char*>(&sig->m_data), 0x1800))
        return 0;
    register int characNo = sig->m_characNo;
    db->set_query(
        "upDate charac_achievement set achievement='%s', last_update_time=now() where charac_no=%u",
        db->blob_to_str(0, buf, (int)bufLen), characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_achievement(charac_no, achievement, last_update_time) values(%u, '%s', now())",
            (unsigned int)sig->m_characNo, db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}