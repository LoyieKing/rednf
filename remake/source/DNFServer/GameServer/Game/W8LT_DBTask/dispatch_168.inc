// ============================================================================
// DB_SaveInformNotice::dispatch —— ORIG 0x08446fcc（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → GetOutBuffer<InformNoticeFlagData>（+0x00
// m_id、+0x04 inform_notice 字符串）→ char buf[256]={'\0'}（ORIG rep stos）
// → escape_string(buf, sig+4) → set_query("upDate login_common set
// inform_notice = '%s' where m_id = %s", buf, NumberToString(m_id,0))；
// exec(true) 失败返 0；getAffectedRowCount()==0 → set_query("inSert into
// login_common(m_id, inform_notice) values(%s, '%s')", midStr, buf)，
// exec 失败返 0；全绿返 1。无基类 dispatch 前置检查。
// ============================================================================
bool DB_SaveInformNotice::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    // 执行序照 ORIG：GetDBHandle → GetOutBuffer → rep stos(buf 清零)。
    // buf 为最大局部（256B），gcc 置于帧底（-0x110），声明后置不影响槽位。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    // P89：单一视图表达式，避免 raw/sig 双局部槽（ORIG 仅一个 sig 槽 -0xc）。
    DB_InformNoticeView* sig = reinterpret_cast<DB_InformNoticeView*>(
        stream->GetOutBuffer<InformNoticeFlagData>());
    char buf[256] = { '\0' };   // -0x110（ORIG rep stos 清零，P4）
    db->escape_string(buf, sig->m_informNotice);
    db->set_query("upDate login_common set inform_notice = '%s' where m_id = %s",
                  buf, NumberToString(sig->m_mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into login_common(m_id, inform_notice) values(%s, '%s')",
            NumberToString(sig->m_mId, 0), buf);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}