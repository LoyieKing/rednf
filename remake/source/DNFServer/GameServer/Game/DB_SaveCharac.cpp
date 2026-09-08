// ============================================================================
// df_game_r 还原 —— DB_SaveCharac 实现（ORIG 10 符号）
// 逐符号转写自 docs/class_func_reports/DB_SaveCharac/（汇编+反编译 C 全文）。
// SQL 字符串、执行顺序、错误码（MySQL::getDBError()==0x426 → 转 upDate 重试）
// 与 ORIG 指令一一对应；SIG_SAVE_CHARAC 字段语义见 SigTypes_D.h（pack(1)，
// sizeof 0x10d，本批字段化）。
// 唯一 owner：本 TU。ORIG 地址核对（nm --defined-only df_game_r）：
//   C1/C2 0x08415c2e | D1/D2 0x08415c4a | D0 0x08415c7a | dispatch 0x08415c98
//   _SaveContractOfCubePremiumInfo 0x08415dbe | _SaveCharacInfo 0x08415eda
//   _SaveCharacStat 0x084163f2 | SendResult 0x08416904 | _SaveCharacManageInfo 0x0841690e
// ============================================================================

#include "DB_SaveCharac.h"

#include "GlobalData.h"             // s_db_mgr / s_psimpleSSO
#include "DBMgr.h"                  // DBMgr::GetDBHandle
#include "MySQL.h"                  // MySQL::set_query/exec/getDBError/getAffectedRowCount/blob_to_str
#include "LogManager.h"             // LogManager::logFormat
#include "CSimpleSSO.h"             // CSimpleSSO::updateSSOExpireTime / stUpdateSSOExpireTime_t
#include "CEnvironment.h"           // DB_HANDLE_2 / E_SERVER_GROUP_NONE
#include "SigTypes.h"               // SIG_SAVE_CHARAC
#include "Stream.h"                 // Stream::GetOutBuffer

// ===========================================================================
// C1/C2 — DB_SaveCharac.md @0x08415c2e：基类 ctor + 设 vptr。
// D1/D2/D0 — ~.md @0x08415c4a/@0x08415c7a：设 vptr + 基类析构；D0 另 delete。
// 非 inline 定义发射全套生命周期符号。
// ===========================================================================
DB_SaveCharac::DB_SaveCharac() : DBDispatcher()
{
}

DB_SaveCharac::~DB_SaveCharac()
{
}

// ===========================================================================
// SendResult — SendResult.md @0x08416904：恒返 1（0x0a 字节）。
// ===========================================================================
int DB_SaveCharac::SendResult(int userIdx, int slot, SIG_SAVE_CHARAC* sig)
{
    return 1;
}

// ===========================================================================
// dispatch — dispatch.md @0x08415c98
// 脏块标志驱动四保存步骤；ManageInfo 失败仅 logFormat 不中断。
// ===========================================================================
bool DB_SaveCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_CHARAC* sig = stream->GetOutBuffer<SIG_SAVE_CHARAC>();
    if (sig == 0)
        return false;
    if (sig->m_bSaveInfo != 0 || sig->m_bSaveInfoSub != 0)
    {
        if (_SaveCharacInfo(sig) != 1)
            return false;
    }
    if (sig->m_bSaveManageInfo != 0)
    {
        if (_SaveCharacManageInfo(sig) != 1)
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x3f7d,
                                  "[SAVE CHARAC MANAGE INFO ERROR] characNo(%u), "
                                  "tagCharacNo(%u), skillIndex(%u)",
                                  sig->m_characNo, sig->m_tagCharacNo,
                                  (unsigned int)sig->m_strikerSkillIndex);
        }
    }
    if (sig->m_bSaveStat != 0)
    {
        if (_SaveCharacStat(sig) != 1)
            return false;
    }
    if (sig->m_bSaveCubePremiumInfo != 0)
        _SaveContractOfCubePremiumInfo(*sig);
    return true;
}

// ===========================================================================
// _SaveContractOfCubePremiumInfo — 报告 @0x08415dbe
// 0x10a 标志未置直接成功；exec 失败返 0；受影响行数==0 时转 inSert。
// ===========================================================================
bool DB_SaveCharac::_SaveContractOfCubePremiumInfo(SIG_SAVE_CHARAC const& sig)
{
    if (sig.m_bSaveCubePremiumInfo != 1)
        return true;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("upDate cube_premium set selected=%u, cube_type=%u "
                  "where charac_no=%u",
                  (unsigned int)sig.m_cubeSelected, (unsigned int)sig.m_cubeType,
                  sig.m_characNo);
    if (!db->exec(true))
        return false;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into cube_premium(charac_no, selected, cube_type) "
                      "values(%u,%u,%u)",
                      sig.m_characNo, (unsigned int)sig.m_cubeSelected,
                      (unsigned int)sig.m_cubeType);
        if (!db->exec(true))
            return false;
    }
    return true;
}

// ===========================================================================
// _SaveCharacManageInfo — 报告 @0x0841690e
// 先 inSert；重复键（getDBError()==0x426）转 upDate 重试；exec 失败返 0。
// ===========================================================================
bool DB_SaveCharac::_SaveCharacManageInfo(SIG_SAVE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("inSert into charac_manage_info(charac_no, tag_charac_no, "
                  "striker_skill_index, max_equip_level) values(%u,%u,%u,%u)",
                  sig->m_characNo, sig->m_tagCharacNo,
                  (unsigned int)sig->m_strikerSkillIndex,
                  (unsigned int)sig->m_maxEquipLevel);
    bool r = db->exec(true);
    if (r != 1)
    {
        if (db->getDBError() == 0x426)
        {
            db->set_query("upDate charac_manage_info set tag_charac_no=%u, "
                          "striker_skill_index=%u, max_equip_level=%u "
                          "where charac_no=%u",
                          sig->m_tagCharacNo,
                          (unsigned int)sig->m_strikerSkillIndex,
                          (unsigned int)sig->m_maxEquipLevel, sig->m_characNo);
            r = db->exec(true);
            if (r != 1)
                return false;
        }
        else
        {
            return false;
        }
    }
    return true;
}

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
