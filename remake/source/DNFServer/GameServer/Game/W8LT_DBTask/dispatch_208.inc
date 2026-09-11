// ============================================================================
// DB_InsertOnTimeRewardUserLog::dispatch —— ORIG 0x08442a56（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_D,0) → *stream >> m_id(uint) >> recv_no(int)；
// 表前缀常量 "1112" → set_query(
// "inSert into event_%s_ontime_reward_user(m_id,recv_no) values(%s,%u)")；
// exec 失败 → cMyTrace(__PRETTY_FUNCTION__, 0xa439, 5) + failed 日志 → 返 0。
bool DB_InsertOnTimeRewardUserLog::dispatch(int /*userIdx*/, int /*slot*/,
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    int recvNo = 0;
    *stream >> mId;
    *stream >> recvNo;
    const char* table = "1112";   // ORIG DAT_08c56b43
    db->set_query(
        "inSert into event_%s_ontime_reward_user(m_id,recv_no) values(%s,%u)",
        table, NumberToString(mId, 0), recvNo);
    if (db->exec(true) != 1)
    {
        // 技巧3：临时链式（具名块对象使 mId/recvNo 槽内收 16B；ORIG
        // 0x08442a56 mId=-0x24/recvNo=-0x28/trace=-0x34?，先例
        // DB_LoadBloodBestRecord 闭合形）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa439, 5)(
            "inSert into event_%s_ontime_reward_user(m_id,recv_no) failed at m_id(%s,%u) ",
            table, NumberToString(mId, 0), recvNo);
        return 0;
    }
    return 1;
}