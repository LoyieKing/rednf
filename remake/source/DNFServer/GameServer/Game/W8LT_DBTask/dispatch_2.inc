bool DB_LoadAccountCargo::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int mId = 0;
    MySQL* db;
    SIG_ACCOUNT_CARGO_DATA* sig;
    int binLen;
    unsigned int outLen;
    char buf[0x100f] = { '\0' };
    static const int compressed_buff_size = 0x100f;  // ORIG _ZZ…DB_LoadAccountCargo…E20compressed_buff_size

    *stream >> mId;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect capacity, money, cargo from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
        return 1;
    if (db->fetch() != 1)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c53), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<SIG_ACCOUNT_CARGO_DATA>();
    memset(sig, 0, 0xd60);
    if (db->get_uint(0, sig->m_capacity) != 1)
        return 0;
    if (db->get_uint(1, sig->m_money) != 1)
        return 0;
    if (db->get_binary(2, buf, compressed_buff_size) != 1)
        return 0;
    binLen = db->get_binary_length(2);
    outLen = 0xd58;
    if (uncompress_zip((char*)sig->m_cargo, outLen, buf,
                       static_cast<unsigned int>(binLen)) != 1)
        return 0;
    if (outLen == 0 || outLen % 0xd58 == 0)
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    return 0;
