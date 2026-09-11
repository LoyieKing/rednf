// ===========================================================================
// _SaveCharacStat — 报告 @0x084163f2
// 45 列 upDate charac_stat（格式串 1017 字节，自 ORIG .rodata @0xc010b4 提取）；
// exec 成功后回写 SSO 到期时间（m_ssoId + 五字段 → updateSSOExpireTime）。
// ===========================================================================
bool DB_SaveCharac::_SaveCharacStat(SIG_SAVE_CHARAC* sig)
{
    // [b28d-46 判词·AE] 302/302 指令数全等、p167 调用点差集 CLEAR（5/5）。
    // `bool r;` 全头部声明（P212）使 db=-0x1c、r=-0x1d 与 ORIG 逐槽一致 ⇒ AE-EQUAL。
    // ORIG 0x084163f2。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_stat set village=%d,exp=%d,HP=%d,fatigue=%d,"
        "premium_fatigue=%d,last_play_time=from_unixtime(%d),used_fatigue=%d,"
        "tutorial_flag=%d,trade_gold_total=%d,"
        "trade_gold_total_billion=trade_gold_total_billion+%d,trade_gold_daily=%d,"
        "help_abuse_ratio=%d,help_abuse_exp=%d,chaos_point=%d,chaos_exp=%d,"
        "chaos_mode_count=%d,chaos_kill_count=%d,chaos_die_count=%d,"
        "chaos_die_time=from_unixtime(%d),assault_count=%d,luck_point=%d,"
        "dungeon_play_count=%d,chaos_kill_time=from_unixtime(%d),"
        "expert_job_exp=%d,open_flag=%d,fatigue_battery_charging=%d,"
        "escalade_tutorial_flag='%s',power_war_point=%d,village_prev=%d,"
        "power_war_assault_count=%d,power_war_assault_victory_count=%d,"
        "last_play_time_powerwar=from_unixtime(%d),fatigue_grownup_buff=%d, "
        "emotion=%d, add_slot_flag=%d, member_dungeon_flag=%d, "
        "member_bonus_fatigue=%d, last_play_dungeon_index=%u, "
        "add_equipslot_flag=%d, channel_equipslot_switch=%d, "
        "expand_equipslot_switch=%d, visible_flags=%d, growth_power_reward=%d, "
        "chaos_respon_time=from_unixtime(%d) where charac_no = %u",
        (int)sig->m_village, sig->m_exp, (unsigned int)sig->m_hp,
        (int)sig->m_fatigue, (unsigned int)sig->m_premiumFatigue,
        sig->m_lastPlayTime, (unsigned int)sig->m_usedFatigue, sig->m_tutorialFlag,
        sig->m_tradeGoldTotal, sig->m_tradeGoldTotalBillion, sig->m_tradeGoldDaily,
        sig->m_helpAbuseRatio, sig->m_helpAbuseExp, sig->m_chaosPoint,
        sig->m_chaosExp, sig->m_chaosModeCount, sig->m_chaosKillCount,
        sig->m_chaosDieCount, sig->m_chaosDieTime, sig->m_assaultCount,
        sig->m_luckPoint, sig->m_dungeonPlayCount, sig->m_chaosKillTime,
        sig->m_expertJobExp, (int)sig->m_openFlag, (int)sig->m_fatigueBatteryCharging,
        db->blob_to_str(0, sig->m_escaladeTutorialFlag, 0x10),
        (int)sig->m_powerWarPoint, (int)sig->m_villagePrev,
        sig->m_powerWarAssaultCount, sig->m_powerWarAssaultVictoryCount,
        sig->m_lastPlayTimePowerwar, (unsigned int)sig->m_fatigueGrownupBuff,
        (int)sig->m_emotion, (int)sig->m_addSlotFlag,
        (unsigned int)sig->m_memberDungeonFlag, (int)sig->m_memberBonusFatigue,
        (unsigned int)sig->m_lastPlayDungeonIndex, (int)sig->m_addEquipslotFlag,
        (int)sig->m_channelEquipslotSwitch, (int)sig->m_expandEquipslotSwitch,
        (int)sig->m_visibleFlags, (int)sig->m_growthPowerReward,
        sig->m_chaosResponTime, sig->m_characNo);
    r = db->exec(true);
    if (r != 1)
        return false;
    WongWork::CSimpleSSO::stUpdateSSOExpireTime_t sso;
    sso.m_reportCnt = sig->m_reportCnt;                    // +0xea
    sso.m_tradeGoldDaily = sig->m_ssoTradeGoldDaily;       // +0xee
    sso.m_rating = sig->m_rating;                          // +0xf6 float 直拷（ORIG movl）
    sso.m_tutorialSkipable = sig->m_tutorialSkipable;      // +0xfa
    sso.m_dungeonGainGold = sig->m_dungeonGainGold;        // +0xf2
    if (GlobalData::s_psimpleSSO->updateSSOExpireTime(sig->m_ssoId, sso) != 1)
        return false;
    return true;
}