// ORIG 0x0843febe：GetDBHandle(DB_HANDLE_2, 0)；两 switch 以 (int)(char)
// 符号扩展进 upDate charac_stat；尾 bool 槽(-0xd 先声明)+P101 `if(!r)`
// 失败出口（xor $1/test/je/mov0/jmp/mov1）。
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
}