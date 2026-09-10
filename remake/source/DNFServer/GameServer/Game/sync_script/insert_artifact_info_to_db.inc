bool CSyncScript::insert_artifact_info_to_db(STEquipmentScript* script)
{
    // ORIG 0x08624b8e：未启用直接 return true；启用则向 dnf_artifact_info 写入
    // it_id（script+0）与 m_artifact738（script+0x738 起 11 列）。
    if (m_field0 != 1)
    {
        return true;
    }
    /* 声明序 = ORIG 槽序（p@-0x10 先、db@-0xc 后；4.1/4.4 标量槽向相反，
     * N93 标量例外翻转声明序即可，探针 /tmp/l5_mul4.cpp）。 */
    const STEquipmentScript::artifact_info& art = script->m_artifact738;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)5,
                                                  (ENUM_SERVER_GROUP)0);
    if (!db->set_query(
            "inSert into dnf_artifact_info(it_id, creature_min_level, "
            "physical_attack, magical_attack, skill_consume_mp_rate, "
            "skill_charge_time_rate, skill_overcharge_time_rate, "
            "experience_amount, physical_critical_hit, magical_critical_hit, "
            "stuck) values(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f)",
            (int)script->m_field0, art.m_minLevel, art.m_physAttack,
            art.m_magAttack, art.m_mpRate, art.m_chargeRate,
            art.m_overChargeRate, art.m_expRate, art.m_physCrit,
            art.m_magCrit, art.m_stuck))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
