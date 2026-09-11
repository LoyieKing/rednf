// ============================================================================
// DB_SaveGameOption_1 —— ORIG dispatch 0x0842ff4e。
// 报告：docs/class_func_reports/DB_SaveGameOption_1/dispatch.md。
// ============================================================================
bool DB_SaveGameOption_1::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序照 ORIG 栈槽（低→高）：buf(-0x86) bufLen(-0x24) db(-0x20) sig(-0x1c)。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_GAME_OPTION_1* sig =
        stream->GetOutBuffer<SIG_SAVE_GAME_OPTION_1>();
    static const int compressedBufferSize = 0x62;  // ORIG _ZZ…DB_SaveGameOption_1…E20compressedBufferSize
    char buf[0x62] = { '\0' };
    unsigned int bufLen = compressedBufferSize;
    if (!compress_zip(buf, bufLen, (const char*)sig + 4, 0x48))
        return 0;
    db->set_query("upDate %s set option_1='%s' where m_id=%s",
                  get_member_game_option(),
                  db->blob_to_str(0, buf, (int)bufLen),
                  NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query("inSert into %s(m_id, option_1) values(%s,'%s')",
                      get_member_game_option(),
                      NumberToString(sig->m_mId, 0),
                      db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}