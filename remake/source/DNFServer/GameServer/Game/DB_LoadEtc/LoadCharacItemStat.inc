// ============================================================================
// DB_LoadEtc::LoadCharacItemStat @0x08445660 —— cooltime/effect blob 双列 +
// check_flag 压缩列（cap 5000）入 +0xd740。错行号 0xac8a/0xac91/0xac98。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x08445660（orig=151/ours=151，槽全等）。
// 已修（本波）：①`bool ok;` 声明前置于 db（ORIG ok=-0x11 深于 db=-0x10，P212）；
//   ②三处 get 结果复用同一 ok 槽（ORIG 三处 `mov %al,-0x11; movzbl; xor $1; test`；
//   原 get_binary/get_compressed 直测少 2 条槽往返）。
// 已修（N107 翻正）：cooltime 实参 `sig->m_cooltimeItem`（数组名退化）4.4 恒
//   `add $0x230c,%eax; mov %eax,%edx`；改 **`sig->m_contItemCooltime.m_info.m_raw`**（显式取首元素地址）
//   即复现 ORIG `lea 0x230c(%eax),%edx`——N107 的可翻位（与 effect 的 `&m_effectItem[0]` 同形）。
// 已修（前波）：exec/fetch 具名 ok + col 槽取后自增 + 三处 trace 临时链。
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
    ok = db->get_binary(col++, sig->m_contItemCooltime.m_info.m_raw, 0x194);  // cooltime_item +0x230c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xac8a, 5)(
            "DB_LoadEtc::LoadCharacItemStat cooltime, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    ok = db->get_binary(col++, sig->m_contItemEffect.m_info.m_raw, 0x194);    // effect_item +0x24a4
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
}