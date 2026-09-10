bool DB_SaveGameOption_ChattingEmoticon::dispatch(int userIdx, int slot,
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    ChattingEmoticonView* sig = reinterpret_cast<ChattingEmoticonView*>(
        stream->GetOutBuffer<SIG_SAVE_CHATTING_EMOTICON>());
    char* blob = 0;
    if (!put_compressed_blob_data(
        db, 0, (char*)((unsigned long)sig + 4UL), 0x210, &blob))
        return 0;
    db->set_query(
        "upDate %s set shortcut_emoticon='%s' where m_id=%s",
        get_member_game_option(), blob, NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into %s(m_id, shortcut_emoticon) values(%s,'%s')",
            get_member_game_option(), NumberToString(sig->m_mId, 0), blob);
        if (!db->exec(true))
            return 0;
    }
    return 1;
