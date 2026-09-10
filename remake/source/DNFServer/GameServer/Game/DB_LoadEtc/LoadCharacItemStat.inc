bool DB_LoadEtc::LoadCharacItemStat(SIG_LOAD_ETC* sig)
{
    // ORIG 槽序：ok(-0x11) 深于 db(-0x10) ⇒ 原码 ok 先声明（P212「先声明深」）；
    // 三处 get 结果均复用该槽（exec/fetch/get_binary/get_compressed）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect cooltime_item, effect_item, check_flag from"
                  " charac_item_stat where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);                                     // ORIG -0x11 具名槽
    if (!ok)
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    ok = db->fetch();
    if (!ok)
    {
        return false;
    }

    int col = 0;                                             // ORIG -0xc 列号槽
    ok = db->get_binary(col++, &sig->m_cooltimeItem[0], 0x194);  // cooltime_item +0x230c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xac8a, 5)(
            "DB_LoadEtc::LoadCharacItemStat cooltime, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    ok = db->get_binary(col++, &sig->m_effectItem[0], 0x194);    // effect_item +0x24a4
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xac91, 5)(
            "DB_LoadEtc::LoadCharacItemStat effect_item, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    ok = get_compressed_blob_data(db, col++, sig->m_itemDictionary, 5000, 0);
    if (!ok)
    {                                                        // check_flag
        cMyTrace(__PRETTY_FUNCTION__, 0xac98, 5)(
            "DB_LoadEtc::LoadCharacItemStat item_dictionary, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    return true;
