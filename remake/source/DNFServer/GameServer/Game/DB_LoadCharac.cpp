// ============================================================================
// df_game_r 还原 —— DB_LoadCharac.cpp
// 目标：清零 /tmp/only_orig/DB_LoadCharac.txt 全部 13 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadCharac/<fn>.md（汇编为准）忠实转写。
// SIG_LOAD_CHARAC 权威定义见 SigTypes_C.h（+0xc CHARAC_DATA / +0x686 tag
// charac no / +0x68b TAG_CHARAC_DATA）。SIG 内 CHARAC_DATA/TAG_CHARAC_DATA
// 各区（village/exp/…）按 ORIG 报告 get_* callee 重载实证类型具名字段访问
// （2026-09-05 §4.3 字段化，绑定别名见 SigTypes_C.h/CCharacter.h 注释）。
// ============================================================================

#include <string.h>
#include <string>

#include "DB_LoadCharac.h"

#include "SigTypes_C.h"       // SIG_LOAD_CHARAC / TAG_CHARAC_DATA / CHARAC_DATA
#include "MySQL.h"
#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "DBMgr.h"             // GetDBHandle
#include "Stream.h"            // Stream::GetOutBuffer 模板
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"         // cMyTrace
#include "CDataManager.h"      // G_CDataManager / get_event_script_mng
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng（is_eventing/get_db_table_name）
#include "GameWorld.h"         // G_GameWorld()->IsSchoolPvPChannel
#include "SyncScriptDeps.h"    // toTString(const std::string&)

// ORIG T _Z14NumberToStringji @0x0810904b（C++ linkage；定义归既有批次）。
extern char* NumberToString(unsigned int value, int radix);

// ============================================================================
// DB_LoadCharac::DB_LoadCharac @0x844d8c6（C1/C2 同址）
// ORIG：调 DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1/D2 同址 0x844d912 设 vptr→基类空 dtor→恒假 delete
// 分支；D0 0x844d912 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
DB_LoadCharac::DB_LoadCharac()
{
}

// ============================================================================
// ~DB_LoadCharac @0x844d912（D1/D2）/ deleting dtor @0x844d912（D0）
// ============================================================================
DB_LoadCharac::~DB_LoadCharac()
{
}

// ============================================================================
// DB_LoadCharac::_getGameDB @0x08408c0a
// 直读 GlobalData::s_db_mgr → GetDBHandle(2, 0)，返回 MySQL*（本函数体不
// 用 this，但 ORIG 调用点 _getCharacInfo 仍按成员压 this，故非 static）。
// ============================================================================
MySQL* DB_LoadCharac::_getGameDB()
{
    return GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                             (ENUM_SERVER_GROUP)0);
}

// ============================================================================
// DB_LoadCharac::_getCharacInfo @0x08408cf4
// charac_info 33 列装载：get_n_rows()==1 才继续；exec/fetch/任一 get 失败
// 返 0；col32(guild_secede) 后按 tag_charac_no(+0x686) 非零装载
// TAG_CHARAC_DATA(+0x68b)（_GetTagCharacInfo 成功才置 1）。
// ============================================================================
bool DB_LoadCharac::_getCharacInfo(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = _getGameDB();
    db->set_query(
        "seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,"
        "phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,"
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight,"
        " max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,"
        "member_flag,element_resist,spec_property,m_id,expert_job, "
        "skill_tree_index, event_charac_level, guild_secede from charac_info "
        "where charac_no=%d and delete_flag!=1",
        sig->m_characNo);
    if (!db->exec(true) || !db->fetch())
        return false;
    if (db->get_n_rows() != 1)
        return false;

    // 逐列 get_* 引用绑定具名字段（SigTypes_C.h union 别名类型 = 下列
    // ORIG callee 重载形参类型，_getCharacInfo.md 汇编逐条实证）。
    if (!db->get_int(0, sig->m_characData.m_characNo))       // charac_no（0811692c int&）
        return false;
    if (!db->get_str(1, sig->m_characData.m_name, 0x1d))     // charac_name
        return false;
    if (!db->get_byte(2, sig->m_characData.m_job))           // job（080fde54 char&）
        return false;
    if (!db->get_short(3, sig->m_characData.m_lev))          // lev（080fddec short&）
        return false;
    if (!db->get_byte(4, sig->m_characData.m_growType))      // grow_type（char&）
        return false;
    if (!db->get_uint(5, sig->m_characData.m_add.m_maxHP))   // maxHP（080e22f2 uint&）
        return false;
    if (!db->get_uint(6, sig->m_characData.m_add.m_maxMP))   // maxMP
        return false;
    if (!db->get_ushort(7, sig->m_characData.m_add.m_field08))   // phy_attack（ushort&）
        return false;
    if (!db->get_ushort(8, sig->m_characData.m_add.m_field0a))   // phy_defense
        return false;
    if (!db->get_ushort(9, sig->m_characData.m_add.m_field0c))   // mag_attack
        return false;
    if (!db->get_ushort(10, sig->m_characData.m_add.m_field0e))  // mag_defense
        return false;
    if (!db->get_int(11, sig->m_characData.m_add.m_field3a))     // inven_weight（int&）
        return false;
    if (!db->get_short(12, sig->m_characData.m_add.m_hpRegen))   // hp_regen（short&）
        return false;
    if (!db->get_short(13, sig->m_characData.m_add.m_mpRegen))   // mp_regen（short&）
        return false;
    if (!db->get_uint(14, sig->m_characData.m_add.m_moveSpeed))  // move_speed（uint&）
        return false;
    if (!db->get_ushort(15, sig->m_characData.m_add.m_field46))  // attack_speed
        return false;
    if (!db->get_ushort(16, sig->m_characData.m_add.m_field48))  // cast_speed
        return false;
    if (!db->get_short(17, sig->m_characData.m_add.m_hitRecovery)) // hit_recovery（short&）
        return false;
    if (!db->get_short(18, sig->m_characData.m_add.m_jump))  // jump（short&）
        return false;
    if (!db->get_int(19, sig->m_characData.m_add.m_field4e)) // charac_weight（int&）
        return false;
    if (!db->get_ushort(20, sig->m_characData.m_field2e))    // max_fatigue（ushort&）
        return false;
    if (!db->get_str(21, sig->m_characData.m_field34, 0x18)) // create_time
        return false;
    if (!db->get_uint(22, sig->m_characData.m_guildIdUint))  // guild_id（uint&）
        return false;
    if (!db->get_ubyte(23, sig->m_characData.m_field5a))     // guild_right（uchar&）
        return false;
    if (!db->get_ushort(24, sig->m_characData.m_field5d))    // max_premium_fatigue
        return false;
    if (!db->get_ubyte(25, sig->m_characData.m_field5f))     // member_flag
        return false;
    if (!db->get_binary(26, sig->m_characData.m_add.m_elementResist, 8)) // element_resist
        return false;
    if (!db->get_binary(27, sig->m_characData.m_add.m_specProperty, 0x22)) // spec_property
        return false;
    if (!db->get_uint(28, sig->m_characData.m_unkB2.m_mId))  // m_id（uint&）
        return false;
    // expert_job：ORIG @0x0844d73a = MySQL::get_byte(int, int&)（非 char& 重载）
    // ——宽度修正：绑定 int m_field106（原 *(char*) 绑到 0x080fde54，callee 不符）。
    if (!db->get_byte(29, sig->m_characData.m_unkB2.m_field106)) // expert_job（int&）
        return false;
    if (!db->get_byte(30, sig->m_characData.m_unkB2.m_skillTreeIndex)) // skill_tree_index（char&）
        return false;
    if (!db->get_byte(31, sig->m_characData.m_unkB2.m_eventCharacLevel)) // event_charac_level（char&）
        return false;
    if (!db->get_byte(32, sig->m_characData.m_unkB2.m_b124)) // guild_secede（char&）
        return false;

    if (sig->m_tagCharacNo != 0)
    {
        if (!_GetTagCharacInfo(sig->m_tagCharacNo, &sig->m_tagCharacData))
            return false;
    }
    return true;
}

// ============================================================================
// DB_LoadCharac::_getCharacStat @0x084093f6
// charac_stat 43 列装载（42 数据列）：exec/fetch/任一 get 失败返 0；
// col9(trade_gold_daily) 后、col10(open_flag) 前将 +0xd9 清零（ORIG 写点）。
// ============================================================================
bool DB_LoadCharac::_getCharacStat(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = _getGameDB();
    db->set_query(
        "seLect village,exp,HP,fatigue,premium_fatigue,"
        "unix_timestamp(last_play_time),used_fatigue,tutorial_flag,"
        "trade_gold_total,trade_gold_daily,open_flag,chaos_point,chaos_exp,"
        "chaos_mode_count,chaos_kill_count,chaos_die_count,"
        "unix_timestamp(chaos_die_time),assault_count,luck_point,"
        "dungeon_play_count,unix_timestamp(chaos_kill_time),help_abuse_ratio,"
        "help_abuse_exp,expert_job_exp,fatigue_battery_charging,"
        "escalade_tutorial_flag,power_war_point,village_prev,"
        "power_war_assault_count,power_war_assault_victory_count,"
        "unix_timestamp(last_play_time_powerwar),fatigue_grownup_buff,emotion,"
        "add_slot_flag,member_dungeon_flag,member_bonus_fatigue,"
        "last_play_dungeon_index, add_equipslot_flag, channel_equipslot_switch,"
        " expand_equipslot_switch, visible_flags, growth_power_reward, "
        "unix_timestamp(chaos_respon_time) from charac_stat where charac_no=%u",
        sig->m_characNo);
    if (!db->exec(true) || !db->fetch())
        return false;

    // 绑定字段类型 = _getCharacStat.md 汇编各 callee 重载形参（get_byte@080fde54
    // char& / get_ubyte@080fdeba uchar& / get_short@080fddec short& /
    // get_ushort@08116990 ushort& / get_int@0811692c int& / get_uint@080e22f2 uint&）。
    if (!db->get_byte(0, sig->m_characData.m_village))       // village（char&）
        return false;
    if (!db->get_uint(1, sig->m_characData.m_exp))           // exp（uint&）
        return false;
    if (!db->get_ubyte(2, sig->m_characData.m_unkB2.m_fieldb2)) // HP（uchar&）
        return false;
    if (!db->get_ushort(3, sig->m_characData.m_field2c))     // fatigue（ushort&）
        return false;
    if (!db->get_ushort(4, sig->m_characData.m_field5b))     // premium_fatigue（ushort&）
        return false;
    if (!db->get_uint(5, sig->m_characData.m_lastPlayTime))  // last_play_time（uint&）
        return false;
    if (!db->get_ushort(6, sig->m_characData.m_unkB2.m_fieldb3)) // used_fatigue（ushort&）
        return false;
    if (!db->get_uint(7, sig->m_characData.m_unkB2.m_tutorialFlag)) // tutorial_flag（uint&）
        return false;
    if (!db->get_uint(8, sig->m_characData.m_unkB2.m_tradeGoldTotal)) // trade_gold_total（uint&）
        return false;
    if (!db->get_uint(9, sig->m_characData.m_unkB2.m_tradeGoldDaily)) // trade_gold_daily（uint&）
        return false;
    sig->m_characData.m_unkB2.m_fieldcd = 0;
    if (!db->get_byte(10, sig->m_characData.m_unkB2.m_openFlag)) // open_flag（char&）
        return false;
    if (!db->get_int(11, sig->m_characData.m_unkB2.m_fieldd5)) // chaos_point（int&）
        return false;
    if (!db->get_int(12, sig->m_characData.m_unkB2.m_fieldd9)) // chaos_exp（int&）
        return false;
    if (!db->get_int(13, sig->m_characData.m_unkB2.m_fielddd)) // chaos_mode_count（int&）
        return false;
    if (!db->get_int(14, sig->m_characData.m_unkB2.m_fielde1)) // chaos_kill_count（int&）
        return false;
    if (!db->get_int(15, sig->m_characData.m_unkB2.m_fielde5)) // chaos_die_count（int&）
        return false;
    if (!db->get_int(16, sig->m_characData.m_unkB2.m_fielde9)) // chaos_die_time（int&）
        return false;
    if (!db->get_int(17, sig->m_characData.m_unkB2.m_fielded)) // assault_count（int&）
        return false;
    if (!db->get_int(18, sig->m_characData.m_unkB2.m_fieldf6)) // luck_point（int&）
        return false;
    if (!db->get_int(19, sig->m_characData.m_unkB2.m_fieldfa)) // dungeon_play_count（int&）
        return false;
    if (!db->get_int(20, sig->m_characData.m_unkB2.m_fieldf1)) // chaos_kill_time（int&）
        return false;
    if (!db->get_uint(21, sig->m_characData.m_unkB2.m_helpAbuseRatio)) // help_abuse_ratio（uint&）
        return false;
    if (!db->get_uint(22, sig->m_characData.m_unkB2.m_helpAbuseExp)) // help_abuse_exp（uint&）
        return false;
    if (!db->get_int(23, sig->m_characData.m_unkB2.m_field10a)) // expert_job_exp（int&）
        return false;
    if (!db->get_short(24, sig->m_characData.m_fatigueBattery)) // fatigue_battery_charging（short&）
        return false;
    if (!db->get_binary(25, sig->m_characData.m_unkB2.m_fieldb9, 0x10)) // escalade_tutorial_flag
        return false;
    if (!db->get_short(26, sig->m_characData.m_unkB2.m_powerWarPoint)) // power_war_point（short&）
        return false;
    if (!db->get_byte(27, sig->m_characData.m_villagePrev))  // village_prev（char&）
        return false;
    if (!db->get_uint(28, sig->m_characData.m_unkB2.m_pwAssaultCount)) // power_war_assault_count（uint&）
        return false;
    if (!db->get_uint(29, sig->m_characData.m_unkB2.m_pwAssaultVicCount)) // …victory_count（uint&）
        return false;
    if (!db->get_uint(30, sig->m_characData.m_unkB2.m_lastPlayTimePW)) // last_play_time_powerwar（uint&）
        return false;
    if (!db->get_ushort(31, sig->m_characData.m_field30))    // fatigue_grownup_buff（ushort&）
        return false;
    if (!db->get_short(32, sig->m_characData.m_unkB2.m_emotion)) // emotion（short&）
        return false;
    if (!db->get_byte(33, sig->m_characData.m_unkB2.m_addSlotFlag)) // add_slot_flag（char&）
        return false;
    if (!db->get_byte(34, sig->m_characData.m_unkB2.m_memberDungeonFlag)) // …flag（char&）
        return false;
    if (!db->get_short(35, sig->m_characData.m_unkB2.m_memberBonusFatigue)) // …fatigue（short&）
        return false;
    if (!db->get_uint(36, sig->m_characData.m_unkB2.m_lastPlayDungeonIndex)) // …index（uint&）
        return false;
    if (!db->get_byte(37, sig->m_characData.m_unkB2.m_b129)) // add_equipslot_flag（char&）
        return false;
    if (!db->get_byte(38, sig->m_characData.m_unkB2.m_b12a)) // channel_equipslot_switch（char&）
        return false;
    if (!db->get_byte(39, sig->m_characData.m_unkB2.m_b12b)) // expand_equipslot_switch（char&）
        return false;
    if (!db->get_byte(40, sig->m_characData.m_unkB2.m_b12e)) // visible_flags（char&）
        return false;
    if (!db->get_ubyte(41, sig->m_characData.m_unkB2.m_growthPowerReward)) // …reward（uchar&）
        return false;
    if (!db->get_int(42, sig->m_characData.m_unkB2.m_i130))  // chaos_respon_time（int&）
        return false;

    return true;
}

// ============================================================================
// DB_LoadCharac::_GetTagCharacInfo @0x08409fa6（static，无 this）
// tag_charac_info 23 列装载：exec/fetch 失败或 getAffectedRowCount()==0 返 0；
// 任一 get 失败返 0；全部成功返 1。
// ============================================================================
bool DB_LoadCharac::_GetTagCharacInfo(unsigned int characNo, TAG_CHARAC_DATA* out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect charac_no, charac_name, job, lev, grow_type, skill_tree_index, "
        "maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, "
        "charac_weight from charac_info where charac_no = %d and delete_flag != 1",
        characNo);
    if (!db->exec(true) || !db->fetch())
        return false;
    if (db->getAffectedRowCount() == 0)
        return false;

    // 绑定字段类型 = _GetTagCharacInfo.md 汇编各 callee 重载形参（同
    // _getCharacInfo 口径；m_add 区共用 CCharacter.h _Additioal_info 别名）。
    if (!db->get_int(0, out->m_characNo))         // charac_no（0811692c int&）
        return false;
    if (!db->get_str(1, out->m_name, 0x1d))       // charac_name
        return false;
    if (!db->get_byte(2, out->m_job))             // job（080fde54 char&）
        return false;
    if (!db->get_short(3, out->m_lev))            // lev（080fddec short&）
        return false;
    if (!db->get_byte(4, out->m_growType))        // grow_type（char&）
        return false;
    if (!db->get_byte(5, out->m_skillTreeIndex))  // skill_tree_index（char&）
        return false;
    if (!db->get_uint(6, out->m_add.m_maxHP))     // maxHP（080e22f2 uint&）
        return false;
    if (!db->get_uint(7, out->m_add.m_maxMP))     // maxMP
        return false;
    if (!db->get_ushort(8, out->m_add.m_field08)) // phy_attack（ushort&）
        return false;
    if (!db->get_ushort(9, out->m_add.m_field0a)) // phy_defense
        return false;
    if (!db->get_ushort(10, out->m_add.m_field0c)) // mag_attack
        return false;
    if (!db->get_ushort(11, out->m_add.m_field0e)) // mag_defense
        return false;
    if (!db->get_binary(12, out->m_add.m_elementResist, 8)) // element_resist
        return false;
    if (!db->get_binary(13, out->m_add.m_specProperty, 0x22)) // spec_property
        return false;
    if (!db->get_int(14, out->m_add.m_field3a))   // inven_weight（int&）
        return false;
    if (!db->get_short(15, out->m_add.m_hpRegen)) // hp_regen（short&）
        return false;
    if (!db->get_short(16, out->m_add.m_mpRegen)) // mp_regen（short&）
        return false;
    if (!db->get_uint(17, out->m_add.m_moveSpeed)) // move_speed（uint&）
        return false;
    if (!db->get_ushort(18, out->m_add.m_field46)) // attack_speed
        return false;
    if (!db->get_ushort(19, out->m_add.m_field48)) // cast_speed
        return false;
    if (!db->get_short(20, out->m_add.m_hitRecovery)) // hit_recovery（short&）
        return false;
    if (!db->get_short(21, out->m_add.m_jump))    // jump（short&）
        return false;
    if (!db->get_int(22, out->m_add.m_field4e))   // charac_weight（int&）
        return false;
    return true;
}

// ============================================================================
// DB_LoadCharac::_UpdateSchoolCharac @0x08408c30
// school_member 更新：job(+0x30)/grow_type(+0x33)/lev(+0x31)，
// where school_id=+0x682 and m_id=NumberToString(accId,0) and charac_no=+0x4。
// 返回 exec(true) 结果。
// 报告 08408c6f..08408c98 实证读取形态：movzbl+movsbl（unsigned char lvalue 经
// (char) 转换）与 movzwl+movswl（unsigned short lvalue 经 (short) 转换）→ 绑定
// **旧类型占位字段**（非 char/short 别名，别名绑 char& 供 _getCharacInfo 用）。
// ============================================================================
bool DB_LoadCharac::_UpdateSchoolCharac(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0x10,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate school_member set job=%d,grow_type=%d,lev=%d where "
        "school_id=%d and m_id=%s and charac_no=%d",
        (int)(char)sig->m_characData.m_field24,   // job（uchar→(char) 双扩展）
        (int)(char)sig->m_characData.m_field27,   // grow_type（uchar→(char)）
        (int)(short)sig->m_characData.m_field25,  // lev（ushort→(short) 双扩展）
        sig->m_characData.m_schoolNo,             // school_id（unsigned int 同型直载，
                                                  // ORIG `mov 0x682(%edx),%esi` 无中转）
        NumberToString(sig->m_accId, 0),          // m_id
        sig->m_characNo);                         // charac_no
    // P0/N96：清除存量 register asm("al") 绑定；exec 返回 bool ⇒ 裸形直产
    // ORIG `call; xor $1; test %al; je <mov$1 岛>`（P168a①）。
    if (!db->exec(true))
        return false;
    return true;
}

// ============================================================================
// DB_LoadCharac::_LoadEventBringUpInfo @0x08409d2e
// 事件 0x19b 表装载 use_material_cnt/give_reward_flag 两列：
//   exec 失败 → cMyTrace 0x225b 返 0；get_n_rows()==0 → 返 1（成功无行）；
//   fetch 失败 → 返 0；get_ubyte 失败 → cMyTrace 0x226b/0x2272 返 0；
//   use_material_cnt → +0x139；give_reward_flag != 0 → +0x138 置 1。
// ============================================================================
bool DB_LoadCharac::_LoadEventBringUpInfo(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    std::string tableName(
        ((EventClassify::CEventScriptMng*)G_CDataManager()
             ->get_event_script_mng())
            ->get_db_table_name(0x19b));
    db->set_query(
        "seLect use_material_cnt, give_reward_flag from %s where charac_no=%u "
        "and m_id=%s",
        toTString(tableName), sig->m_characNo,
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x225b, 5);
        trace("DB_LoadCharac::_LoadEventBringUpInfo, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
        return true;

    unsigned char useMaterialCnt = 0;
    unsigned char giveRewardFlag = 0;
    if (!db->fetch())
        return false;
    if (!db->get_ubyte(0, useMaterialCnt))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x226b, 5);
        trace("DB_LoadCharac::_LoadEventBringUpInfo, get_ubyte(1) ERROR "
              "charac_no=%u", sig->m_characNo);
        return false;
    }
    if (!db->get_ubyte(1, giveRewardFlag))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x2272, 5);
        trace("DB_LoadCharac::_LoadEventBringUpInfo, get_ubyte(2) ERROR "
              "charac_no=%u", sig->m_characNo);
        return false;
    }
    sig->m_characData.m_unkB2.m_useMaterialCnt = useMaterialCnt;
    sig->m_characData.m_unkB2.m_giveRewardFlag = (giveRewardFlag != 0) ? 1 : 0;
    return true;
}

// ============================================================================
// DB_LoadCharac::_sendResult @0x0840a4ce
// StreamPool::Acquire("DBThread.cpp",0x233a) → guard<<7<<slot →
// guard->GetInBuffer<SIG_LOAD_CHARAC>() → memcpy(sig,0x703) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)；随后 cMyTrace 0x2347
// "CharLastPlayTick char(%d) t(%d)"（charac_no=+4 / t=+0x59）。
// ============================================================================
void DB_LoadCharac::_sendResult(int userIdx, int slot, SIG_LOAD_CHARAC* sig)
{
    // [b28d-64] P259/P0：ORIG 0x0840a4e0 `call Acquire; mov %eax,%ebx;
    // mov %ebx,0x4(%esp)` 寄存器中转（无 stream 具名槽 -0x10）⇒ 内联实参。
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x233a), true);
        guard.operator*()->operator<<(7);
        guard.operator*()->operator<<(slot);
        SIG_LOAD_CHARAC* out = guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC>();
        memcpy(out, sig, 0x703);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }                                                        // ~CStreamGuard

    // [b28d-64] P227：ORIG 0x0840a5ac 两实参先入 %esi/%ebx 跨 cMyTrace ctor
    // 保活（`mov 0x14(%ebp),%eax; mov 0x59(%eax),%esi; …%ebx`）⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x2347, 0)(
        "CharLastPlayTick char(%d) t(%d)", sig->m_characNo,
        sig->m_characData.m_field4d);
}

// ============================================================================
// DB_LoadCharac::dispatch @0x08408a3c
// DBDispatcher::dispatch 先决（失败返 0）→ GetOutBuffer<SIG_LOAD_CHARAC>()，
// +0x08(m_result) 清零。try{ _getCharacInfo 失败 throw 1；_getCharacStat
// 失败 throw 3；事件 0x19b is_eventing 且 _LoadEventBringUpInfo 失败
// throw 5 } catch(int line){ m_result=line }。_sendResult 后按
// IsSchoolPvPChannel && schoolNo(+0x682)!=0 调 _UpdateSchoolCharac。返 1。
// ============================================================================
bool DB_LoadCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    SIG_LOAD_CHARAC* sig = stream->GetOutBuffer<SIG_LOAD_CHARAC>();
    sig->m_result = 0;

    try
    {
        if (!_getCharacInfo(sig))
        {
            throw 1;
        }
        if (!_getCharacStat(sig))
        {
            throw 3;
        }
        if (((EventClassify::CEventScriptMng*)G_CDataManager()
                 ->get_event_script_mng())
                ->is_eventing(0x19b))
        {
            if (!_LoadEventBringUpInfo(sig))
            {
                throw 5;
            }
        }
    }
    catch (int line)
    {
        sig->m_result = line;
    }

    _sendResult(userIdx, slot, sig);

    if (G_GameWorld()->IsSchoolPvPChannel() && sig->m_characData.m_schoolNo != 0)
    {
        _UpdateSchoolCharac(sig);
    }
    return true;
}
