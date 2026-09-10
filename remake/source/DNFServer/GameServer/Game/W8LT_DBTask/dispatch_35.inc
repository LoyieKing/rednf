bool DB_UpdateNatType::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_NAT_TYPE* sig;
    MySQL* db;
    sig = stream->GetOutBuffer<SIG_NAT_TYPE>();
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_nat_daily(occ_time,nat_type,nat_type_other,success_cnt,fail_cnt) values(from_unixtime(%d),%d,%d,%d,%d)",
        ((DB_NatTypeView*)sig)->m_time,
        ((DB_NatTypeView*)sig)->m_natType,
        ((DB_NatTypeView*)sig)->m_natTypeOther,
        ((DB_NatTypeView*)sig)->m_successCnt,
        ((DB_NatTypeView*)sig)->m_failCnt);
    if (!db->exec(true))
        return 0;
    return 1;
