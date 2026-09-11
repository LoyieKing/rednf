// ============================================================================
// DB_UpdateInGameEventHistory::dispatch —— ORIG 0x08444972（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(9,0) → *stream >> m_id(uint) >> type(int) >> history1(uint)
// >> history2(uint)；upDate event_ingame_history；exec!=1 或 affected==0 →
// inSert 同键；恒返 1。
bool DB_UpdateInGameEventHistory::dispatch(int /*userIdx*/, int /*slot*/,
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    // ORIG 仅 mId(-0x20) 带清零，其余 lea 直传引用无初始化。
    int type;
    unsigned int history1;
    unsigned int history2;
    *stream >> mId;
    *stream >> type;
    *stream >> history1;
    *stream >> history2;
    db->set_query(
        "upDate event_ingame_history set history_1 = %u,history_2 = %u where m_id=%s and type = %d",
        history1, history2, NumberToString(mId, 0), type);
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into event_ingame_history(m_id,type,history_1,history_2) values(%s,%d,%d,%d)",
            NumberToString(mId, 0), type, history1, history2);
        db->exec(true);
    }
    return 1;
}