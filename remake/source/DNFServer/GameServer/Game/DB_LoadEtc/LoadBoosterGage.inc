bool DB_LoadEtc::LoadBoosterGage(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 2;    // ORIG _ZZN10DB_LoadEtc15LoadBoosterGage…E9handleIdx
    // ORIG 槽序：ok(-0x15) 深于 db(-0x14) ⇒ 原码 ok 先声明（P212 全头部声明「先声明深」）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect gage from member_booster_gage where m_id=%u",
                  sig->m_accId);
    ok = db->exec(true);                                     // local_19 保存复用
    if (!ok)
    {
        return false;
    }
    int nRows = db->get_n_rows();                            // ORIG -0x10 具名槽 + cmpl mem
    if (nRows != 1)
    {
        return false;
    }
    if (!ok)                                                 // ORIG 0x08446e31 独立第二守卫
    {
        return false;
    }
    int col = 0;                                             // ORIG -0xc 列号槽（取后自增）
    if (!db->fetch())
    {
        return false;
    }
    // ORIG 0x08446e7a：get_int 结果存回同一 ok 槽（-0x15）再取反（P102 复用）。
    ok = db->get_int(col++, sig->m_boosterGage);             // +0xeccc
    if (!ok)
    {
        // ORIG 0x08446e93：ctor 前 m_accId 装 %ebx 跨 ctor 保活（P227 临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0xafc4, 5)(
            "DB_LoadEtc::LoadBoosterGage, get(%d) ERROR m_id=%u", col,
            sig->m_accId);           // P226 语句表达式实测无增益（20→20），保留 P227 形
        return false;
    }
    return true;
