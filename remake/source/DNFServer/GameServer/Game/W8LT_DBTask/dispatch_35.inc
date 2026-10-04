// ORIG 0x841c74e：GetOutBuffer<SIG_NAT_TYPE>（无基类检查）→ 5 占位 inSert
// log_nat_daily(occ_time,nat_type,nat_type_other,success_cnt,fail_cnt)。
// 声明序 sig→db 与 ORIG 栈槽对应（-0x10/-0xc）；每字段直接经视图强转
// 访问 sig（ORIG 每字段重读 -0x10，P48 无 CSE）。
bool DB_UpdateNatType::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_NAT_TYPE* sig;
    MySQL* db;
    sig = stream->GetOutBuffer<SIG_NAT_TYPE>();
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_nat_daily(occ_time,nat_type,nat_type_other,success_cnt,fail_cnt) values(from_unixtime(%d),%d,%d,%d,%d)",
        sig->m_time,
        sig->m_natType,
        sig->m_natTypeOther,
        sig->m_successCnt,
        sig->m_failCnt);
    if (!db->exec(true))
        return 0;
    return 1;
}