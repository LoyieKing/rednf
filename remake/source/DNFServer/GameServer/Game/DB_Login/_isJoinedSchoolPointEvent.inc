// ============================================================================
// _isJoinedSchoolPointEvent（ORIG T 0x08410f74）
// ============================================================================

bool DB_Login::_isJoinedSchoolPointEvent(unsigned int mid)
{
    if ((bool)(GlobalData::s_systemTime_.getCurSec() > 0x4834a970))
    {
        return false;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect school_id from event_school_point where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    return true;
}
