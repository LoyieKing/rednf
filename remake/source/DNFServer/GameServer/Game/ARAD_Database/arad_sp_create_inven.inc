int CREATE_QUERY::arad_sp_create_inven(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    static const unsigned int kInvenSrcLen = sourceInvenSize;   // ORIG 背包快照长度
    char zipBuf[0x4152];
    memset(zipBuf, 0, sizeof(zipBuf));
    unsigned int zipLen = 0x4152;
    char* invenSrc = reinterpret_cast<char*>(sig) + 0x3a9;

    if (sig->m_giftFindOk != 0)
    {
        // ORIG 判定与 equip 相反：compress 返回非 0 即放弃（逐指令转写，勿"修正"）
        if (compress_zip(zipBuf, zipLen, invenSrc, kInvenSrcLen))
        {
            return -1;
        }
        if (db->insert("inSert into inventory (charac_no, inventory,coin) "
                       "values(%u, '%s',%d)",
                       sig->m_characNo,
                       db->blob_to_str(0, zipBuf, static_cast<int>(zipLen)),
                       db->blob_to_str(0, invenSrc, kInvenSrcLen),
                       WongWork::CEventMgr::GetCoinCharacCreate()) != 0)
        {
            return -1;
        }
    }
    else
    {
        db->insert("inSert into inventory  (charac_no, coin, money)  values(%u, %d) ",
                   sig->m_characNo);
        if (WongWork::CEventMgr::GetCoinCharacCreate() != 0)
        {
            return -1;
        }
    }
    return 0;
