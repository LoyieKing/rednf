// ORIG 0x08439c6c：GetDBHandle(4,0)；GetOutBuffer<SIG_UPDATE_COMPOUND_EMBLEM_
// STATISTIC>；inSert log_emblem_compound(DATE_SUB(CURDATE(),INTERVAL 1 DAY),
// channel_no,grade0..6)；exec 成功且 affected!=0 完成；否则 upDate grade 累加
// （exec 失败返 0）；返 1。
bool DB_UpdateCompoundEmblemStatics::dispatch(int userIdx, int slot,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC* sig =
        stream->GetOutBuffer<SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC>();
    db->set_query(
        "inSert into log_emblem_compound(occ_date, channel_no, grade0, grade1, grade2, grade3, grade4, grade5, grade6) values(DATE_SUB(CURDATE(), INTERVAL 1 DAY), %d, %d, %d, %d, %d, %d, %d, %d)",
        sig->m_userIdx, sig->m_values[0], sig->m_values[1], sig->m_values[2],
        sig->m_values[3], sig->m_values[4], sig->m_values[5], sig->m_values[6]);
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "upDate log_emblem_compound set grade0 = grade0+%d, grade1=grade1+%d, grade2=grade2+%d, grade3=grade3+%d, grade4=grade4+%d, grade5=grade5+%d, grade6=grade6+%d where occ_date=DATE_SUB(CURDATE(), INTERVAL 1 DAY) and channel_no=%d",
            sig->m_values[0], sig->m_values[1], sig->m_values[2], sig->m_values[3],
            sig->m_values[4], sig->m_values[5], sig->m_values[6], sig->m_userIdx);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}