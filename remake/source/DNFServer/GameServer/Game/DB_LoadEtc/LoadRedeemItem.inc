bool DB_LoadEtc::LoadRedeemItem(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc14LoadRedeemItem…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect redeem_info from charac_inven_expand where charac_no=%u",
                  sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b16, 5)(
            "DB_LoadRedeemItemList::dispatch, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b20, 5)(
            "DB_LoadRedeemItemList::dispatch, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    // [b28d-solo-DB_LoadEtc] `={0}` 触发 4.4 内联 rep stos（P214③；原 memset 生 call），
    //   与 LoadPvPMissionList 同法；get_binary 第 3 参复用 outLen 具名槽（P244）。
    char blob[0x6c7] = { 0 };
    unsigned int outLen = 0x6c7;
    if (!db->get_binary(0, blob, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b28, 5)(
            "DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    if (!uncompress_zip(sig->m_expandData6, outLen, blob, srcLen))
    {
        return false;
    }
    return true;
