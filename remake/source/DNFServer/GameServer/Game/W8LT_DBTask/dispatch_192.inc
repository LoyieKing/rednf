bool DB_UpdateItemMakingSkill::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_ITEM_MAKING_SKILL* sig = stream->GetOutBuffer<SIG_ITEM_MAKING_SKILL>();
    unsigned int characNo =
        reinterpret_cast<const DB_ItemMakingSkillView*>(sig)->m_characNo;
    const short* skills =
        reinterpret_cast<const DB_ItemMakingSkillView*>(sig)->m_skillLevels;
    db->set_query(
        "upDate item_making_skill_info set weapon=%d, cloth=%d, leather=%d, light_armor=%d, heavy_armor=%d, plate=%d, amulet=%d, wrist=%d, ring=%d, support=%d, magic_stone=%d where charac_no =%u",
        (int)skills[0], (int)skills[1],
        (int)skills[2], (int)skills[3],
        (int)skills[4], (int)skills[5],
        (int)skills[6], (int)skills[7],
        (int)skills[8], (int)skills[9],
        (int)skills[10], characNo);
    if (db->exec(true) == 1)
    {
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into item_making_skill_info (charac_no, weapon, cloth, leather, light_armor, heavy_armor, plate, amulet, wrist, ring, support, magic_stone) values(%u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d )",
                characNo, (int)skills[0], (int)skills[1], (int)skills[2],
                (int)skills[3], (int)skills[4], (int)skills[5],
                (int)skills[6], (int)skills[7], (int)skills[8],
                (int)skills[9], (int)skills[10]);
            if (db->exec(true) != 1)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x9ff5, 5);
                trace("DB_UpdateItemMakingSkill::InsertItemMakingSkill insert, exec() ERROR charac_no=%u",
                      characNo);
                return 0;
            }
        }
        return 1;
    }
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x9fde, 5);
    trace("DB_UpdateItemMakingSkill::UpdateItemMakingSkill update, exec() ERROR charac_no=%u",
          characNo);
    return 0;
