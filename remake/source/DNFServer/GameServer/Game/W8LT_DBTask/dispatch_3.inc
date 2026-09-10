bool DB_SaveAccountCargo::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int mId = 0;
    SIG_ACCOUNT_CARGO_DATA* sig;
    MySQL* db;

    // ORIG 0x0843b7cd 语句序：mId=0 → >>mId → GetOutBuffer → buf 清零 →
    // bufLen=0x100f（声明序决定槽位；buf 清零在 stream 读之后）。
    *stream >> mId;
    sig = stream->GetOutBuffer<SIG_ACCOUNT_CARGO_DATA>();
    static const int compressed_buff_size = 0x100f;  // ORIG _ZZ…DB_SaveAccountCargo…E20compressed_buff_size
    char buf[0x100f] = { '\0' };
    unsigned int bufLen = compressed_buff_size;
    if (compress_zip(buf, bufLen, sig->m_cargo, 0xd58) != 1)
        return 0;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // ORIG 0x0843b8e2：blob_to_str 无具名槽——实参内联进 set_query（P0/P89）。
    db->set_query("upDate account_cargo set capacity=%u, money=%u, cargo='%s' where m_id = %s",
                  sig->m_capacity, sig->m_money,
                  db->blob_to_str(0, buf, (int)bufLen), NumberToString(mId, 0));
    // ORIG 0x0843b928：`xor $1; test; je; mov $0/%1,%eax` ⇒ P101 早返形。
    if (!db->exec(true))
        return 0;
    return 1;
