// ORIG 0x0818b68c：inSert into charac_quest 全空任务行。
int CREATE_QUERY::arad_sp_create_charac_quest(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (db->insert(
            "inSert into charac_quest(charac_no,quest_10,quest_15,quest_20,quest_30,"
            "quest_40,quest_50, quest_60,quest_70,quest_etc,play_1,play_1_trigger,"
            "play_2,play_2_trigger,play_3,play_3_trigger,play_4,play_4_trigger,"
            "play_5,play_5_trigger,play_6,play_6_trigger,play_7,play_7_trigger,"
            "play_8,play_8_trigger,play_9,play_9_trigger,play_10,play_10_trigger) "
            "values(%d,'','','','','','','','','',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)",
            sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}