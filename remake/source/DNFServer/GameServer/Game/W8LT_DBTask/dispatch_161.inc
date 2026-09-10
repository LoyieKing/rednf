bool DB_UpdateBloodBestRecord::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_BloodBestRecordView* sig = reinterpret_cast<DB_BloodBestRecordView*>(
        stream->GetOutBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>());
    db->set_query(
        "upDate charac_blood_best_record set best_round=%d, best_time=%u where charac_no=%u and dungeon_index=%d",
        sig->m_bestRound, sig->m_bestTime, sig->m_characNo,
        sig->m_dungeonIndex);
    // ORIG 0x08431c74：needInsert 无槽——4.1 以 %eax 物化双常数岛
    // `mov $1; jmp T / mov $0 / T: test al; je`（首段 xor$1;test;jne =
    // exec!=1 直落 true 路）。[b28d-solo] P226/P316 语句表达式零槽短路物化
    // （`if (({ A || B; }))`）复现该形；P0 合规（无 register 绑定、无具名局部）。
    if (({ db->exec(true) != 1 || db->getAffectedRowCount() == 0; }))
    {
        db->set_query(
            "inSert into charac_blood_best_record(charac_no,dungeon_index,best_round,best_time) values(%u,%d,%d,%u)",
            sig->m_characNo, sig->m_dungeonIndex, sig->m_bestRound,
            sig->m_bestTime);
        if (!db->exec(true))
            return 0;
        return 1;
    }
    return 1;
