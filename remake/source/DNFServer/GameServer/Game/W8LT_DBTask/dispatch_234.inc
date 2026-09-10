bool DB_SaveGameOption_QuickChatting::dispatch(int userIdx, int slot,
                                               Stream* stream)
{
    static const int compressedBufferSize_QuickChat = 0x2dc;  // ORIG _ZZ…QuickChatting…E30compressedBufferSize_QuickChat
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    QuickChattingView* sig = reinterpret_cast<QuickChattingView*>(
        stream->GetOutBuffer<SIG_SAVE_GAME_OPTION_QUICKCHAT>());
    // ORIG 0x08430250：`lea -0x300,%ebx; mov $0xb7; rep stos` 内联清零
    // （P4 = 声明初始化数组形，4.4 对 `= {0}` 亦内联）；尺寸 0x2dc。
    char buf[0x2dc] = { 0 };
    unsigned int compressedLen = compressedBufferSize_QuickChat;
    if (!compress_zip(buf, compressedLen, sig->m_data, 0x258))
        return 0;
    db->set_query(
        "upDate %s set option_3='%s' where m_id=%s",
        get_member_game_option(), db->blob_to_str(1, buf, (int)compressedLen),
        NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into %s(m_id, option_3) values(%s,'%s')",
            get_member_game_option(), NumberToString(sig->m_mId, 0),
            db->blob_to_str(0, buf, (int)compressedLen));
        if (!db->exec(true))
            return 0;
    }
    return 1;
