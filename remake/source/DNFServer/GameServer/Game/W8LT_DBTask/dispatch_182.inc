// ORIG 0x0843a264：GetDBHandle(1,0)；GetOutBuffer<SIG_SECURITY_CARD_SERVICE_
// VALIDITYTIME>；+8(ushort)==8 → security_card_validity_time upDate；==0x10 →
// goblin_validity_time upDate（均含 validity_ip='%s' uid@+0xa）；exec(true) 返 1。
bool DB_SecuServiceUpdateValidityTime::dispatch(int userIdx, int slot,
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_SERVICE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>();
    if (sig->m_flag == 8)
    {
        db->set_query(
            "upDate member_security_grade set security_card_validity_time\t=unix_timestamp(now())+%d , validity_ip = '%s' where m_id=%s",
            sig->m_type,
            // ORIG +0x59 `lea 0xa(%edx),%ecx`：validity_ip 取 sig+10（数组
            // decay 取地址复现单条 lea；`+10` 指针算术会成 add+mov 两条）。
            (const char*)&((char*)sig)[10],
            NumberToString(sig->m_mId, 0));
    }
    else if (sig->m_flag == 0x10)
    {
        db->set_query(
            "upDate member_security_grade set goblin_validity_time\t\t=unix_timestamp(now())+%d , validity_ip = '%s' where m_id=%s",
            sig->m_type,
            (const char*)&((char*)sig)[10],
            NumberToString(sig->m_mId, 0));
    }
    db->exec(true);
    return 1;
}