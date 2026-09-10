bool DB_TOD_Reset_UserAPC::GetAPCUserInfo(_APCInfo& info)
{
    static const int handleIdx = 3;
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                   E_SERVER_GROUP_NONE);
    MySQL* db8 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                   E_SERVER_GROUP_NONE);
    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                   E_SERVER_GROUP_NONE);
    db2->set_query(
        "seLect charac_name,lev,job,grow_type,guild_id from charac_info where charac_no=%d",
        info.m_userCharacNo);
    db2->exec(true);
    if (!db2->exec(true) || !db2->fetch())
        return 0;
    if (db2->get_n_rows() != 1)
        return 0;
    // ORIG 0x0844062x 用具名列索引：`movl $0x0,-0x38; mov -0x38,%eax;
    // addl $0x1,-0x38; mov %eax,0x4(%esp)`（每处 2 条，共 5 处）。
    int col = 0;
    if (!db2->get_str(col++, info.m_userCharacName, 0x1e))
        return 0;
    if (!db2->get_byte(col++, info.m_lv))
        return 0;
    if (!db2->get_byte(col++, info.m_job))
        return 0;
    if (!db2->get_byte(col++, info.m_growType))
        return 0;
    unsigned int& guildId = info.m_guildId;
    if (!db2->get_uint(col++, guildId))
        return 0;
    if (guildId != 0)
    {
        int serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        db8->set_query("seLect guild_name from guild_info where guild_id=%d",
                       guildId);
        if (!db8->exec(true) || !db8->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f46, 5);
            trace("TOD:select guild db fail, guild_id:%d, server_id:%d",
                  guildId, serverId);
        }
        else if (db8->get_n_rows() != 0 &&
                 !db8->get_str(0, info.m_guildName, 0x16))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f4e, 5);
            trace("TOD:select guild db fail, guild_id:%d, server_id:%d",
                  guildId, serverId);
        }
    }
    int triples[0x73][3];   // slot/it_id/clear_avatar_id × 115（0x564 字节）
    memset(triples, 0, sizeof(triples));
    db3->set_query(
        "seLect slot,it_id,clear_avatar_id from user_items where charac_no=%u and expire_date>now() and stat=0 limit %d",
        (unsigned int)info.m_userCharacNo, 0x73);
    if (!db3->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x9f5d, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
              (unsigned int)info.m_userCharacNo);
        return 0;
    }
    int nRows = db3->get_n_rows();
    for (int i = 0; i < nRows; ++i)
    {
        if (!db3->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f67, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            break;
        }
        if (!db3->get_int(0, triples[i][0]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f6d, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(0) ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            return 0;
        }
        if (!db3->get_int(1, triples[i][1]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f73, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(1) ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            return 0;
        }
        if (!db3->get_int(2, triples[i][2]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f79, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(2) ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            return 0;
        }
        if (triples[i][0] >= 0 && triples[i][0] <= 9)
            info.m_abilities[triples[i][0]] = triples[i][1];
    }
    for (int j = 0; j < nRows; ++j)
    {
        if (triples[j][2] != 0)
        {
            db3->set_query("seLect it_id from user_items where ui_id=%d",
                           triples[j][2]);
            if (db3->exec(true) && db3->fetch())
                db3->get_int(0, info.m_abilities[triples[j][0]]);
        }
    }
    char buf[0x37a] = { '\0' };          // ORIG rep stos 清零（P4）
    unsigned int outLen = 0x37a;
    db3->set_query("seLect equipslot from inventory where charac_no=%u",
                   (unsigned int)info.m_userCharacNo);
    db3->exec(true);
    db3->fetch();
    int binLen = db3->get_binary_length(0);
    if (db3->get_binary(0, buf, 0x37a))
    {
        Inven_Item items[22];            // ORIG 构造 0x15→-1 共 22 次
        if (uncompress_zip((char*)&items[10], outLen, buf,
                           (unsigned int)binLen))
        {
            info.m_abilities[10] =
                reinterpret_cast<DB_EquipslotBlobView*>(&items[10])->m_dword2;
            db3->set_query(
                "seLect it_id,name from creature_items where slot=238 and charac_no=%u and expire_date > now()",
                (unsigned int)info.m_userCharacNo);
            if (!db3->exec(true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x9fbf, 5);
                trace("TOD: read DB creature_items error");
            }
            else if (db3->fetch())
            {
                db3->get_int(0, info.m_creatureId);
                db3->get_str(1, info.m_creatureName, 0xc);
            }
            return 1;
        }
        return 0;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x9f9d, 5);
    trace("DB_LoadInventory::GetUserInven, get_binary(3) ERROR charac_no=%u",
          (unsigned int)info.m_userCharacNo);
    return 0;
