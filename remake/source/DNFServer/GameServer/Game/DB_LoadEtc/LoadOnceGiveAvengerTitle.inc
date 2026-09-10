bool DB_LoadEtc::LoadOnceGiveAvengerTitle(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    // 结果不入槽：ORIG 8904 内联实参直传（无 accStr 保存对）。
    db->set_query("seLect m_id from event_1107_avenger_plan where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_flagPresentAvengerTitle = 1;                 // +0xc9ae movb $1 直写
        return false;
    }
    int nRows = db->get_n_rows();       // 后声明→浅 -0xc（ORIG cmpl $0,-0xc 保存比较形）
    if (nRows != 0)
        sig->m_flagPresentAvengerTitle = 1;           // movb $1,0xc9ae(%eax)
    else
        sig->m_flagPresentAvengerTitle = 0;           // movb $0,0xc9ae(%eax)
    return true;
