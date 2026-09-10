bool DB_UpdateBoosterGageData::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_BoosterGageView* sig = reinterpret_cast<DB_BoosterGageView*>(
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
