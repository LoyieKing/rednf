// ORIG 0x841c7f6：GetOutBuffer<SIG_NAT_TYPE_USER> → 3 占位 inSert
// log_nat_stat(occ_time,nat_type,nat_cnt)。声明序 sig→db（-0x10/-0xc）。
bool DB_UpdateNatTypeUser::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_NAT_TYPE_USER* sig;
    MySQL* db;
    sig = stream->GetOutBuffer<SIG_NAT_TYPE_USER>();
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_nat_stat(occ_time,nat_type,nat_cnt) values(from_unixtime(%d),%d,%d)",
        sig->m_time, sig->m_type, sig->m_count);
    if (!db->exec(true))
        return 0;
    return 1;
}