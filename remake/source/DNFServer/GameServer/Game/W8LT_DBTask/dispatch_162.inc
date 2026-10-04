// ============================================================================
// DB_UpdateBoosterGageData::dispatch —— ORIG 0x08446eec（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8446eec：GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<
// SIG_BOOSTER_GAGE_DATA>（+0=m_dwAccountNo/+4=m_dwBoosterGage）→ upDate
// member_booster_gage set gage=%u where m_id=%u；exec 失败返 0；成功且
// getAffectedRowCount()==0 → 补 inSert member_booster_gage(m_id, gage)
// values(%u, %u)，insert exec 失败返 0；成功（或 update 影响行）返 1。
// ============================================================================
bool DB_UpdateBoosterGageData::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_BOOSTER_GAGE_DATA* sig = (
        stream->GetOutBuffer<SIG_BOOSTER_GAGE_DATA>());
    db->set_query("upDate member_booster_gage set gage=%u where m_id=%u",
                  sig->m_dwBoosterGage, sig->m_dwAccountNo);
    // ORIG +0x6d `xor $1;test;je body; mov $0;jmp end` = P16/P59 early-return
    // `if (!exec) return 0;`（两次 exec 均此形），非 if-else 嵌套。
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into member_booster_gage(m_id, gage) values(%u, %u)",
            sig->m_dwAccountNo, sig->m_dwBoosterGage);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}