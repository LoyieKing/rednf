bool DB_LoadEtc::_LoadQuestShop(unsigned int characNo, CHARAC_QUEST_SHOP_DATA* out)
{
    static const int handleIdx = 2;    // ORIG _ZZN10DB_LoadEtc14_LoadQuestShop…E9handleIdx
    bool ret;                   // ORIG local_19 @-0x15(%ebp)：exec/fetch/qp/init_count
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    int idx;                    // ORIG local_14 @-0x10(%ebp)：列号，随每次 get 自增
    db->set_query("seLect qp, init_count, max_hp, max_mp, psy_attack, psy_defense,"
                  " mag_attack, mag_defence, move_speed, attack_speed, hp_regen,"
                  " mp_regen, all_element_resist, fire_element_resist,"
                  " water_element_resist, light_element_resist, dark_element_resist,"
                  " all_element_attack,fire_element_attack, water_element_attack,"
                  " light_element_attack, dark_element_attack, psy_critical,"
                  " mag_critical, good_hit , evasion , hit_recovery ,"
                  " separate_psy_mag_attack , quest_piece from charac_quest_shop"
                  " where charac_no=%u", characNo);
    ret = db->exec(true);
    if (!ret)
    {
        return false;
    }
    if (db->getAffectedRowCount() == 0)
    {
        return true;
    }
    ret = db->fetch();
    if (!ret)
    {
        return false;
    }

    // 列序/目标字段/失败抛出行号逐项对齐汇编（local_14 自 0 起、取后自增）。
    idx = 0;
    ret = db->get_uint(idx++, out->m_qp);                       // qp          +0x04
    if (!ret)
    {
        // b28d-31: P221 全表达式临时链（ORIG 0x08404bff 槽 -0x38 在 trq2 的
        // -0x28 之前；具名声明 4.4 槽序反向 ⇒ 改构造即调用）。
        cMyTrace("static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                 " CHARAC_QUEST_SHOP_DATA*)", 0x17ea, 5)(
            "DB_LoadEtc::LoadQuestShope, get(%d) ERROR charac_no=%u", idx,
            characNo);
        return false;
    }
    ret = db->get_uint(idx++, out->m_initCount);                // init_count  +0x08
    if (!ret)
    {
        // b28d-31: P221 全表达式临时链（ORIG 0x08404c22 槽 -0x28）。
        cMyTrace("static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                 " CHARAC_QUEST_SHOP_DATA*)", 0x17f1, 5)(
            "DB_LoadEtc::LoadQuestShope, get(%d) ERROR charac_no=%u", idx,
            characNo);
        return false;
    }
    // ORIG EH 区间自首个 throw 起：仅逐列 get_ushort/get_uint 段位于 try 内。
    try
    {
