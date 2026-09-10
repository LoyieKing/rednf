bool DB_AvatarOptionChange::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    DB_AvatarOptionChangeView* sig =
        reinterpret_cast<DB_AvatarOptionChangeView*>(
            stream->GetOutBuffer<SIG_AVATAR_OPTION_CHANGE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set ability_no=%d where ui_id=%d and charac_no=%d",
        (unsigned int)sig->m_characNo, sig->m_uiId, sig->m_abilityNo);
    if (db->exec(true) != 1)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xae5c, 5);
        trace("AVATAR_OPTION_CHAGE::DB_UPDATE_FAIL");
        return 0;
    }
    return 1;
