// ===========================================================================
// _SaveCharacInfo — 报告 @0x08415eda
// 标志组合三分支：(4&&6) 全 24 列；(4&&!6) 主 22 列（无 premium/guild/event）；
// (!4&&6) 仅 premium/guild/event 三列。exec 成功判定即返回值。
// ===========================================================================
bool DB_SaveCharac::_SaveCharacInfo(SIG_SAVE_CHARAC* sig)
{
    // [b28d-46 判词·N/P75] 345/345 指令数全等、p167 调用点差集 CLEAR（9/9）、db=-0x1c
    // 与 r=-0x1d 逐槽一致；残差仅两分支各一处 blob_to_str varargs 实参求值序
    // （ORIG 4.1 左先 elementResist→specProperty / 4.4 右先，暂存槽 -0x5c/-0x58 vs
    // -0x9c/-0x98），两次调用无副作用、语义等价 ⇒ P75 跨版本结构性 DIFF 封顶。
    // ORIG 0x08415eda。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    if (sig->m_bSaveInfo != 0 && sig->m_bSaveInfoSub != 0)
    {
        db->set_query(
            "upDate charac_info set lev=%d,grow_type=%d,maxHP=%d,maxMP=%d,"
            "phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,"
            "inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,"
            "attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,"
            "charac_weight=%d,element_resist='%s',spec_property='%s',"
            "max_premium_fatigue=%d,guild_right=%d,expert_job=%d,"
            "skill_tree_index=%d, event_charac_level=%d where charac_no = %u",
            (int)sig->m_lev, (int)sig->m_growType, sig->m_maxHP, sig->m_maxMP,
            (unsigned int)sig->m_phyAttack, (unsigned int)sig->m_phyDefense,
            (unsigned int)sig->m_magAttack, (unsigned int)sig->m_magDefense,
            sig->m_invenWeight, (int)sig->m_hpRegen, (int)sig->m_mpRegen,
            sig->m_moveSpeed, (unsigned int)sig->m_attackSpeed,
            (unsigned int)sig->m_castSpeed, (int)sig->m_hitRecovery,
            (int)sig->m_jump, sig->m_characWeight,
            db->blob_to_str(1, sig->m_elementResist, 0x22),
            db->blob_to_str(0, sig->m_specProperty, 8),
            (unsigned int)sig->m_maxPremiumFatigue, (unsigned int)sig->m_guildRight,
            (int)sig->m_expertJob, (int)sig->m_skillTreeIndex,
            (int)sig->m_eventCharacLevel, sig->m_characNo);
    }
    else if (sig->m_bSaveInfo != 0)
    {
        db->set_query(
            "upDate charac_info set lev=%d,grow_type=%d,maxHP=%d,maxMP=%d,"
            "phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,"
            "inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,"
            "attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,"
            "charac_weight=%d,element_resist='%s',spec_property='%s', "
            "expert_job=%d, skill_tree_index=%d where charac_no = %u",
            (int)sig->m_lev, (int)sig->m_growType, sig->m_maxHP, sig->m_maxMP,
            (unsigned int)sig->m_phyAttack, (unsigned int)sig->m_phyDefense,
            (unsigned int)sig->m_magAttack, (unsigned int)sig->m_magDefense,
            sig->m_invenWeight, (int)sig->m_hpRegen, (int)sig->m_mpRegen,
            sig->m_moveSpeed, (unsigned int)sig->m_attackSpeed,
            (unsigned int)sig->m_castSpeed, (int)sig->m_hitRecovery,
            (int)sig->m_jump, sig->m_characWeight,
            db->blob_to_str(1, sig->m_elementResist, 0x22),
            db->blob_to_str(0, sig->m_specProperty, 8),
            (int)sig->m_expertJob, (int)sig->m_skillTreeIndex, sig->m_characNo);
    }
    else if (sig->m_bSaveInfoSub != 0)
    {
        db->set_query(
            "upDate charac_info set max_premium_fatigue=%d,guild_right=%d,"
            "event_charac_level=%d where charac_no = %u",
            (unsigned int)sig->m_maxPremiumFatigue, (unsigned int)sig->m_guildRight,
            (int)sig->m_eventCharacLevel, sig->m_characNo);
    }
    r = db->exec(true);
    if (r != 1)
        return false;
    return true;
}