// ===========================================================================
// _CreateNewCharacStat — 报告 @0x0840215e
// ===========================================================================
bool DB_CreateCharac::_CreateNewCharacStat(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into charac_stat (charac_no,HP, forbidden_to_play, forbidden_due_to) values(%d,100,0,0)",
        sig->m_characNo);
    if (!db->exec(true))
    {
        // N96 清理：register asm("ebx") 绑定删除。ORIG 为 4.1 ctor 前求值并
        // %ebx 跨调用保活（N80 三探针不可翻，P197② 语句表达式亦回退帧漂）；
        // 取合法裸内联形（NDIFF 最优），语义与调用点压参同值。
        cMyTrace(__PRETTY_FUNCTION__, 0x10fb, 5)(
            "DB_CreateCharac::_CreateNewCharacStat, exec() ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        sig->m_resultCode = 2;
        return false;
    }
    return true;
}