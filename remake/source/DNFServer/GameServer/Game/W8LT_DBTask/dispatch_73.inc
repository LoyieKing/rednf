// ============================================================================
// DB_SaveTitleBook::dispatch —— ORIG 0x0840db00（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<stTitleBook_Save> → 四段压缩（general 0x10ae→
// 0x1410、specific 0x2fa8→0x393c、despair 0x1c98→0x225c、event 0xe4c→
// 0x1134，源 sig+0x00/0x10ae/0x4056/0x5cee），任一失败返 0 → upDate
// charac_titlebook（四 blob，charac_no=+0x6b3a）exec 失败返 0；affected==0
// 补 inSert；inSert exec 失败返 0；全绿返 1。
bool DB_SaveTitleBook::dispatch(int userIdx, int slot, Stream* stream)
{
    DB_TitleBookSaveView* sig =
        (DB_TitleBookSaveView*)stream->GetOutBuffer<stTitleBook_Save>();
    char general[0x1410] = { 0 };
    unsigned int generalLen = 0x1410;
    if (!compress_zip(general, generalLen, sig->m_data, 0x10ae))
        return 0;
    char specific[0x393c] = { 0 };
    unsigned int specificLen = 0x393c;
    if (!compress_zip(specific, specificLen, sig->m_data + 0x10ae, 0x2fa8))
        return 0;
    char despair[0x225c] = { 0 };
    unsigned int despairLen = 0x225c;
    if (!compress_zip(despair, despairLen, sig->m_data + 0x4056, 0x1c98))
        return 0;
    char event[0x1134] = { 0 };
    unsigned int eventLen = 0x1134;
    if (!compress_zip(event, eventLen, sig->m_data + 0x5cee, 0xe4c))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_titlebook set general_section='%s', specific_section='%s', despair='%s', event='%s' where charac_no=%u",
        db->blob_to_str(0, general, (int)generalLen),
        db->blob_to_str(1, specific, (int)specificLen),
        db->blob_to_str(2, despair, (int)despairLen),
        db->blob_to_str(3, event, (int)eventLen),
        (unsigned int)sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_titlebook(charac_no, general_section, specific_section, despair, event) values(%u, '%s','%s','%s','%s')",
            (unsigned int)sig->m_characNo,
            db->blob_to_str(0, general, (int)generalLen),
            db->blob_to_str(1, specific, (int)specificLen),
            db->blob_to_str(2, despair, (int)despairLen),
            db->blob_to_str(3, event, (int)eventLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}