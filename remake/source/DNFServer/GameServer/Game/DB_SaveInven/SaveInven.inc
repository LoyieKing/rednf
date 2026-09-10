bool DB_SaveInven::SaveInven(SIG_SAVE_INVENTORY* sig)
{
    static const int handleIdx = 3;
    bool r;
    MySQL* db;
    char equipslot[0x37a] = { 0 };
    char inventory[0x473e] = { 0 };
    char creature[0x1d36] = { 0 };
    unsigned int equipslotLen = 0x37a;
    unsigned int inventoryLen = 0x473e;
    unsigned int creatureLen = 0x1d36;

    // §4.3 续：equipslot +0x019 = m_invenType0、inventory +0x557 =
    // m_invenType1、creature +0x710b = m_invenType3[140]
    //（(0x710b-0x4faf)/0x3d == 140 整除实证；sourceCreatureSize 0x184e =
    //  102×0x3d ⇒ 覆盖 m_invenType3[140..237] + m_creatureTail[0..3]，
    //  末址 0x8959 == &m_cargoCapacity，三段首尾与具名字段精确闭合）。
    if (sig->m_saveFlag == 0 || sig->m_saveFlag == 2)
    {
        try
        {
            if (compress_zip(equipslot, equipslotLen, (char*)sig->m_invenType0,
                             ARAD::DATABASE::sourceEquipSize) != 1)
                throw (unsigned int)0x4427;
            if (compress_zip(inventory, inventoryLen, (char*)sig->m_invenType1,
                             ARAD::DATABASE::sourceInvenSize) != 1)
                throw (unsigned int)0x4429;
            if (compress_zip(creature, creatureLen,
                             (char*)&sig->m_invenType3[140],
                             ARAD::DATABASE::sourceCreatureSize) != 1)
                throw (unsigned int)0x4430;
        }
        catch (unsigned int e)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4438, 5)("Compress failed. charac_no(%d)(%d)", sig->m_characNo, e);
            return 0;
        }
    }

    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    if (sig->m_saveFlag == 0)
    {
        db->set_query("upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d,equipslot='%s',inventory='%s',creature='%s' where charac_no=%u",
                      sig->m_money, sig->m_coin, sig->m_payCoin, sig->m_eventCoin,
                      db->blob_to_str(0, equipslot, equipslotLen),
                      db->blob_to_str(1, inventory, inventoryLen),
                      db->blob_to_str(2, creature, creatureLen),
                      sig->m_characNo);
    }
    else if (sig->m_saveFlag == 1)
    {
        db->set_query("upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d  where charac_no=%u",
                      sig->m_money, sig->m_coin, sig->m_payCoin, sig->m_eventCoin,
                      sig->m_characNo);
    }
    else
    {
        db->set_query("upDate inventory set money=%u,equipslot='%s',inventory='%s',creature='%s' where charac_no=%u",
                      sig->m_money,
                      db->blob_to_str(0, equipslot, equipslotLen),
                      db->blob_to_str(1, inventory, inventoryLen),
                      db->blob_to_str(2, creature, creatureLen),
                      sig->m_characNo);
    }

    r = db->exec(true);
    if (r != 1)
        return 0;

    db->set_query("inSert into member_avatar_coin (m_id , avatar_coin ) values (%s , %u) on duplicate key upDate avatar_coin = %u ",
                  NumberToString(sig->m_accId, 0), sig->m_avatarCoin, sig->m_avatarCoin);
    r = db->exec(true);
    if (r != 1)
        return 0;
    return 1;
