// ============================================================================
// GetCheckPickUpRandomOptionItem（ORIG T 0x08414e7a）
// ============================================================================

int DB_Login::GetCheckPickUpRandomOptionItem(SIG_LOGIN_DATA* sig)
{
    sig->m_checkPickUpRandomOptionItemFlag = false;      // +0x3fa8
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect check_count from check_pick_up_random_option_item where m_id = %s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows != 0)
    {
        if (!db->fetch())
        {
            return 0;
        }
        if (!db->get_ubyte(0, reinterpret_cast<unsigned char&>(sig->m_checkPickUpRandomOptionItemFlag)))
        {
            return 0;
        }
    }
    return 1;
}
