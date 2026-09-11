// ---------------------------------------------------------------------------
// DB_UpdateQuestShopBuyingStatusList::dispatch —— ORIG 0x0840568a
// ---------------------------------------------------------------------------
// ORIG：GetDBHandle(2,0) → GetOutBuffer<SIG_QUEST_SHOP_DATA> → upDate
// charac_quest_shop（29 列，实参序见下）→ exec 失败返 0；affected==0 时补
// inSert（30 列）→ inSert exec 失败返 0；全绿返 1。
// 字段映射（ORIG 逐偏移 movzwl/mov 实证）：m_qp(+4)/m_initCount(+8) uint；
// statusCounts 各槽 ushort；m_questPiece(+0xa6)/m_characNo(+0) uint。
bool DB_UpdateQuestShopBuyingStatusList::dispatch(int userIdx, int slot,
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x084056b3-0x084056c7：GetOutBuffer 结果先落临时槽 -0x20，
    // 再拷贝进 sig(-0x1c)（两跳具名中转形，4.4 直存会少一条）。
    SIG_QUEST_SHOP_DATA* rawSig = stream->GetOutBuffer<SIG_QUEST_SHOP_DATA>();
    SIG_QUEST_SHOP_DATA* sig = rawSig;
    db->set_query(
        "upDate charac_quest_shop set qp=%u, init_count=%u, max_hp=%d, max_mp=%d, psy_attack=%d, psy_defense=%d, mag_attack=%d, mag_defence=%d, move_speed=%d, attack_speed=%d, hp_regen=%d, mp_regen=%d, all_element_resist=%d, fire_element_resist=%d, water_element_resist=%d, light_element_resist=%d, dark_element_resist=%d, all_element_attack=%d,fire_element_attack=%d, water_element_attack=%d, light_element_attack=%d, dark_element_attack=%d, psy_critical=%d, mag_critical=%d, good_hit=%d, evasion=%d, hit_recovery=%d, separate_psy_mag_attack=%d, quest_piece=%d where charac_no=%u",
        sig->m_main.m_qp, sig->m_main.m_initCount,
        sig->m_main.m_data.m_maxHp, sig->m_main.m_data.m_maxMp,
        sig->m_main.m_data.m_psyAttack,
        sig->m_main.m_data.m_psyDefense, sig->m_main.m_data.m_magAttack,
        sig->m_main.m_data.m_magDefence, sig->m_main.m_data.m_moveSpeed,
        sig->m_main.m_data.m_attackSpeed, sig->m_main.m_data.m_hpRegen,
        sig->m_main.m_data.m_mpRegen, sig->m_main.m_data.m_allElementResist,
        sig->m_main.m_data.m_fireElementResist,
        sig->m_main.m_data.m_waterElementResist,
        sig->m_main.m_data.m_lightElementResist,
        sig->m_main.m_data.m_darkElementResist,
        sig->m_main.m_data.m_allElementAttack,
        sig->m_main.m_data.m_fireElementAttack,
        sig->m_main.m_data.m_waterElementAttack,
        sig->m_main.m_data.m_lightElementAttack,
        sig->m_main.m_data.m_darkElementAttack,
        sig->m_main.m_data.m_psyCritical, sig->m_main.m_data.m_magCritical,
        sig->m_main.m_data.m_goodHit, sig->m_main.m_data.m_evasion,
        sig->m_main.m_data.m_hitRecovery,
        sig->m_main.m_data.m_separatePsyMagAttack,
        sig->m_main.m_questPiece, sig->m_main.m_field0);
    // P168a①：ORIG 首段 `[call exec][xor$1][test %al][je 岛]` + 岛
    // `mov$0,%eax; jmp EPI` = bool callee `!` 裸 early-return（技巧1）。
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
                "inSert into charac_quest_shop(charac_no, qp, init_count, max_hp, max_mp, psy_attack, psy_defense, mag_attack, mag_defence, move_speed, attack_speed, hp_regen, mp_regen, all_element_resist, fire_element_resist, water_element_resist, light_element_resist, dark_element_resist, all_element_attack,fire_element_attack, water_element_attack, light_element_attack, dark_element_attack, psy_critical, mag_critical, good_hit , evasion , hit_recovery , separate_psy_mag_attack , quest_piece) values(%u, %u, %u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                sig->m_main.m_field0, sig->m_main.m_qp, sig->m_main.m_initCount,
                sig->m_main.m_data.m_maxHp, sig->m_main.m_data.m_maxMp,
                sig->m_main.m_data.m_psyAttack,
                sig->m_main.m_data.m_psyDefense,
                sig->m_main.m_data.m_magAttack,
                sig->m_main.m_data.m_magDefence,
                sig->m_main.m_data.m_moveSpeed,
                sig->m_main.m_data.m_attackSpeed,
                sig->m_main.m_data.m_hpRegen,
                sig->m_main.m_data.m_mpRegen,
                sig->m_main.m_data.m_allElementResist,
                sig->m_main.m_data.m_fireElementResist,
                sig->m_main.m_data.m_waterElementResist,
                sig->m_main.m_data.m_lightElementResist,
                sig->m_main.m_data.m_darkElementResist,
                sig->m_main.m_data.m_allElementAttack,
                sig->m_main.m_data.m_fireElementAttack,
                sig->m_main.m_data.m_waterElementAttack,
                sig->m_main.m_data.m_lightElementAttack,
                sig->m_main.m_data.m_darkElementAttack,
                sig->m_main.m_data.m_psyCritical,
                sig->m_main.m_data.m_magCritical,
                sig->m_main.m_data.m_goodHit, sig->m_main.m_data.m_evasion,
                sig->m_main.m_data.m_hitRecovery,
                sig->m_main.m_data.m_separatePsyMagAttack,
                sig->m_main.m_questPiece);
        // P168a①：二段落同 xor early-return 形。
        if (!db->exec(true))
            return 0;
    }
    return 1;
}