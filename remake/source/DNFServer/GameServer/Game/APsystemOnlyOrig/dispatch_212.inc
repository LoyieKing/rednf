// ============================================================================
// DB_SaveCharacInformNotice::dispatch —— ORIG 0x0844c170（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(2,0) → GetOutBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>（null 返
// 0）→ escape_string(+0x04 flags) → upDate charac_option
// charac_inform_notice；affected==0 → inSert（options/best_clear_time 默认
// 空串）；inSert 失败返 0。
bool DB_SaveCharacInformNotice::dispatch(int /*userIdx*/, int /*slot*/,
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_CHARAC_INFORM_NOTICE* sig =
        stream->GetOutBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>();
    if (sig == 0)
        return 0;
    char esc[0x101] = { '\0' };
    // bool r 占 -0x9 系近槽，自然把 db/sig/esc 外推 4B（ORIG db=-0x14、
    // sig=-0x10、esc=-0x115、r=-0x9）。
    db->escape_string(esc,
                      reinterpret_cast<char*>(sig->m_flags));
    db->set_query(
        "upDate charac_option set charac_inform_notice='%s' where charac_no=%u",
        esc, sig->m_characNo);
    // ORIG 0x0844c…：两处 exec 结果经 bool 槽 -0x9 spill+reload：
    // `mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax; test; je BODY`
    // （4.1 `if(!ok) return 0;` 物化形，技巧8 同族）。
    bool r;
    r = db->exec(true);
    if (!r)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_option(charac_no, options, best_clear_time, blue_marble_enter_count, charac_inform_notice) values (%u, '%s', '%s', 0, '%s')",
            sig->m_characNo, "", "", esc);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    return 1;
}