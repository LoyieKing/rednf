bool DB_UpdateAuraAvatarOption::dispatch(int userIdx, int slot,
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    // ORIG 仅 -0x10（characNo）带清零；optionType/value 直接 lea 传引用。
    int optionType;
    int value;
    *stream >> characNo;
    *stream >> optionType;
    *stream >> value;
    db->set_query(
        "upDate aura_avatar_option set value_1=%d where charac_no=%u and option_type = %d",
        value, characNo, optionType);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into aura_avatar_option(charac_no,option_type,value_1) values(%u,%d,%d)",
            characNo, optionType, value);
        db->exec(true);
    }
    return 1;
}