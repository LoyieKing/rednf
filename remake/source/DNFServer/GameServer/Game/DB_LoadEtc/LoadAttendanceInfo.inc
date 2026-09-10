bool DB_LoadEtc::LoadAttendanceInfo(SIG_ATTENDANCE_EVENT* sig, unsigned int accId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    sig->m_isEnabled = 0;
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x60)->IsEventing((CUser*)0))
    {
        return false;
    }

    long resetBase = GetCurrentResetBaseTime(
        GlobalData::s_systemTime_.getCurSec(),
        G_CEnvironment()->m_serverEnvir.m_fatigueTime /*ORIG +0x37c*/);
    // ORIG 8904a：NumberToString 结果直进实参（无保存槽）。
    db->set_query("seLect occ_date from event_1207_attendance where m_id = %s and"
                  " occ_date > from_unixtime(%u)", NumberToString(accId, 0),
                  resetBase);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();   // ORIG -0xc 保存+cmpl $0（后声明→浅槽）
    if (nRows == 0)
    {
        sig->m_isEnabled = 1;        // 当日尚可签到
    }
    return true;
