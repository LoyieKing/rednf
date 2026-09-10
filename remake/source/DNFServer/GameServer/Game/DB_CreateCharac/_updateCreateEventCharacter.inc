bool DB_CreateCharac::_updateCreateEventCharacter(SIG_CREATE_CHARAC* sig)
{
    // P0/P183：窗口计算右值内联（ORIG 寄存器持有，无 nowSec/inWindow 槽）。
    if (GlobalData::s_systemTime_.getCurSec() > (int)0x4b055d6f &&
        GlobalData::s_systemTime_.getCurSec() <= (int)0x4b3bbf50)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                      E_SERVER_GROUP_NONE);
        db->set_query("upDate member_login set event_charac_flag = 1 where m_id = %s",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
        sig->m_eventCharacFlag = 1;
        return true;
    }
    sig->m_resultCode = 4;
    return false;
