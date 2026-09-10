bool DB_SaveExpandEquipslot::_SaveCharacStat(unsigned int characNo,
                                             SIG_SAVE_EXPAND_CHARAC_STAT* sig)
{
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_stat set channel_equipslot_switch=%d, expand_equipslot_switch=%d where charac_no = %u",
        (int)(char)sig->m_channelEquipslotSwitch,
        (int)(char)sig->m_expandEquipslotSwitch,
        characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
