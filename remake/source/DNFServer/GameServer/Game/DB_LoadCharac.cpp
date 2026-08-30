// ============================================================================
// df_game_r 还原 —— DB_LoadCharac.cpp
// 目标：清零 /tmp/only_orig/DB_LoadCharac.txt 全部 13 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadCharac/<fn>.md（汇编为准）忠实转写。
// SIG_LOAD_CHARAC 权威定义见 SigTypes_C.h（+0xc CHARAC_DATA / +0x686 tag
// charac no / +0x68b TAG_CHARAC_DATA）。SIG 内其余字段（village/exp/…）按
// ORIG 报告各 get_* 写点以文档化偏移直访（同 CUser::GetCharacInfo 约定）。
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

// SIG_LOAD_CHARAC 文档化偏移访问宏（ORIG 无 _ZN12_GLOBAL__N_13SLC 符号，
// 编译产物为直接偏移寻址 → 原实现为宏而非 inline 函数）
#define SLC(sig, off) (reinterpret_cast<char*>(sig) + (off))

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

    if (!db->get_int(0, *(int*)SLC(sig, 0x0c)))              // charac_no
        return false;
    if (!db->get_str(1, SLC(sig, 0x10), 0x1d))               // charac_name
        return false;
    if (!db->get_byte(2, *(char*)SLC(sig, 0x30)))            // job
        return false;
    if (!db->get_short(3, *(short*)SLC(sig, 0x31)))          // lev
        return false;
    if (!db->get_byte(4, *(char*)SLC(sig, 0x33)))            // grow_type
        return false;
    if (!db->get_uint(5, *(unsigned int*)SLC(sig, 0x6c)))    // maxHP
        return false;
    if (!db->get_uint(6, *(unsigned int*)SLC(sig, 0x70)))    // maxMP
        return false;
    if (!db->get_ushort(7, *(unsigned short*)SLC(sig, 0x74)))    // phy_attack
        return false;
    if (!db->get_ushort(8, *(unsigned short*)SLC(sig, 0x76)))    // phy_defense
        return false;
    if (!db->get_ushort(9, *(unsigned short*)SLC(sig, 0x78)))    // mag_attack
        return false;
    if (!db->get_ushort(10, *(unsigned short*)SLC(sig, 0x7a)))   // mag_defense
        return false;
    if (!db->get_int(11, *(int*)SLC(sig, 0xa6)))             // inven_weight
        return false;
    if (!db->get_short(12, *(short*)SLC(sig, 0xaa)))         // hp_regen
        return false;
    if (!db->get_short(13, *(short*)SLC(sig, 0xac)))         // mp_regen
        return false;
    if (!db->get_uint(14, *(unsigned int*)SLC(sig, 0xae)))   // move_speed
        return false;
    if (!db->get_ushort(15, *(unsigned short*)SLC(sig, 0xb2)))   // attack_speed
        return false;
    if (!db->get_ushort(16, *(unsigned short*)SLC(sig, 0xb4)))   // cast_speed
        return false;
    if (!db->get_short(17, *(short*)SLC(sig, 0xb6)))         // hit_recovery
        return false;
    if (!db->get_short(18, *(short*)SLC(sig, 0xb8)))         // jump
        return false;
    if (!db->get_int(19, *(int*)SLC(sig, 0xba)))             // charac_weight
        return false;
    if (!db->get_ushort(20, *(unsigned short*)SLC(sig, 0x3a)))   // max_fatigue
        return false;
    if (!db->get_str(21, SLC(sig, 0x40), 0x18))              // create_time
        return false;
    if (!db->get_uint(22, *(unsigned int*)SLC(sig, 0x62)))   // guild_id
        return false;
    if (!db->get_ubyte(23, *(unsigned char*)SLC(sig, 0x66))) // guild_right
        return false;
    if (!db->get_ushort(24, *(unsigned short*)SLC(sig, 0x69)))   // max_premium_fatigue
        return false;
    if (!db->get_ubyte(25, *(unsigned char*)SLC(sig, 0x6b))) // member_flag
        return false;
    if (!db->get_binary(26, SLC(sig, 0x7c), 8))              // element_resist
        return false;
    if (!db->get_binary(27, SLC(sig, 0x84), 0x22))           // spec_property
        return false;
    if (!db->get_uint(28, *(unsigned int*)SLC(sig, 0x140)))  // m_id
        return false;
    if (!db->get_byte(29, *(char*)SLC(sig, 0x112)))          // expert_job
        return false;
    if (!db->get_byte(30, *(char*)SLC(sig, 0x12a)))          // skill_tree_index
        return false;
    if (!db->get_byte(31, *(char*)SLC(sig, 0x12c)))          // event_charac_level
        return false;
    if (!db->get_byte(32, *(char*)SLC(sig, 0x130)))          // guild_secede
        return false;

    if (*(int*)SLC(sig, 0x686) != 0)   // m_tagCharacNo
    {
        if (!_GetTagCharacInfo(*(unsigned int*)SLC(sig, 0x686),
                               (TAG_CHARAC_DATA*)SLC(sig, 0x68b)))
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

    if (!db->get_byte(0, *(char*)SLC(sig, 0x2e)))            // village
        return false;
    if (!db->get_uint(1, *(unsigned int*)SLC(sig, 0x34)))    // exp
        return false;
    if (!db->get_ubyte(2, *(unsigned char*)SLC(sig, 0xbe)))  // HP
        return false;
    if (!db->get_ushort(3, *(unsigned short*)SLC(sig, 0x38)))    // fatigue
        return false;
    if (!db->get_ushort(4, *(unsigned short*)SLC(sig, 0x67)))    // premium_fatigue
        return false;
    if (!db->get_uint(5, *(unsigned int*)SLC(sig, 0x59)))    // last_play_time
        return false;
    if (!db->get_ushort(6, *(unsigned short*)SLC(sig, 0xbf)))    // used_fatigue
        return false;
    if (!db->get_uint(7, *(unsigned int*)SLC(sig, 0xc1)))    // tutorial_flag
        return false;
    if (!db->get_uint(8, *(unsigned int*)SLC(sig, 0xd5)))    // trade_gold_total
        return false;
    if (!db->get_uint(9, *(unsigned int*)SLC(sig, 0xdd)))    // trade_gold_daily
        return false;
    *(int*)SLC(sig, 0xd9) = 0;
    if (!db->get_byte(10, *(char*)SLC(sig, 0x101)))          // open_flag
        return false;
    if (!db->get_int(11, *(int*)SLC(sig, 0xe1)))             // chaos_point
        return false;
    if (!db->get_int(12, *(int*)SLC(sig, 0xe5)))             // chaos_exp
        return false;
    if (!db->get_int(13, *(int*)SLC(sig, 0xe9)))             // chaos_mode_count
        return false;
    if (!db->get_int(14, *(int*)SLC(sig, 0xed)))             // chaos_kill_count
        return false;
    if (!db->get_int(15, *(int*)SLC(sig, 0xf1)))             // chaos_die_count
        return false;
    if (!db->get_int(16, *(int*)SLC(sig, 0xf5)))             // chaos_die_time
        return false;
    if (!db->get_int(17, *(int*)SLC(sig, 0xf9)))             // assault_count
        return false;
    if (!db->get_int(18, *(int*)SLC(sig, 0x102)))            // luck_point
        return false;
    if (!db->get_int(19, *(int*)SLC(sig, 0x106)))            // dungeon_play_count
        return false;
    if (!db->get_int(20, *(int*)SLC(sig, 0xfd)))             // chaos_kill_time
        return false;
    if (!db->get_uint(21, *(unsigned int*)SLC(sig, 0x10a)))  // help_abuse_ratio
        return false;
    if (!db->get_uint(22, *(unsigned int*)SLC(sig, 0x10e)))  // help_abuse_exp
        return false;
    if (!db->get_int(23, *(int*)SLC(sig, 0x116)))            // expert_job_exp
        return false;
    if (!db->get_short(24, *(short*)SLC(sig, 0x3e)))         // fatigue_battery_charging
        return false;
    if (!db->get_binary(25, SLC(sig, 0xc5), 0x10))           // escalade_tutorial_flag
        return false;
    if (!db->get_short(26, *(short*)SLC(sig, 0x11a)))        // power_war_point
        return false;
    if (!db->get_byte(27, *(char*)SLC(sig, 0x2f)))           // village_prev
        return false;
    if (!db->get_uint(28, *(unsigned int*)SLC(sig, 0x11c)))  // power_war_assault_count
        return false;
    if (!db->get_uint(29, *(unsigned int*)SLC(sig, 0x120)))  // power_war_assault_victory_count
        return false;
    if (!db->get_uint(30, *(unsigned int*)SLC(sig, 0x124)))  // last_play_time_powerwar
        return false;
    if (!db->get_ushort(31, *(unsigned short*)SLC(sig, 0x3c))) // fatigue_grownup_buff
        return false;
    if (!db->get_short(32, *(short*)SLC(sig, 0x128)))        // emotion
        return false;
    if (!db->get_byte(33, *(char*)SLC(sig, 0x12b)))          // add_slot_flag
        return false;
    if (!db->get_byte(34, *(char*)SLC(sig, 0x12d)))          // member_dungeon_flag
        return false;
    if (!db->get_short(35, *(short*)SLC(sig, 0x12e)))        // member_bonus_fatigue
        return false;
    if (!db->get_uint(36, *(unsigned int*)SLC(sig, 0x131)))  // last_play_dungeon_index
        return false;
    if (!db->get_byte(37, *(char*)SLC(sig, 0x135)))          // add_equipslot_flag
        return false;
    if (!db->get_byte(38, *(char*)SLC(sig, 0x136)))          // channel_equipslot_switch
        return false;
    if (!db->get_byte(39, *(char*)SLC(sig, 0x137)))          // expand_equipslot_switch
        return false;
    if (!db->get_byte(40, *(char*)SLC(sig, 0x13a)))          // visible_flags
        return false;
    if (!db->get_ubyte(41, *(unsigned char*)SLC(sig, 0x13b))) // growth_power_reward
        return false;
    if (!db->get_int(42, *(int*)SLC(sig, 0x13c)))            // chaos_respon_time
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

    char* p = out->m_data;
    if (!db->get_int(0, *(int*)(p + 0x00)))       // charac_no
        return false;
    if (!db->get_str(1, p + 0x04, 0x1d))          // charac_name
        return false;
    if (!db->get_byte(2, *(char*)(p + 0x21)))     // job
        return false;
    if (!db->get_short(3, *(short*)(p + 0x22)))   // lev
        return false;
    if (!db->get_byte(4, *(char*)(p + 0x24)))     // grow_type
        return false;
    if (!db->get_byte(5, *(char*)(p + 0x25)))     // skill_tree_index
        return false;
    if (!db->get_uint(6, *(unsigned int*)(p + 0x26)))    // maxHP
        return false;
    if (!db->get_uint(7, *(unsigned int*)(p + 0x2a)))    // maxMP
        return false;
    if (!db->get_ushort(8, *(unsigned short*)(p + 0x2e)))    // phy_attack
        return false;
    if (!db->get_ushort(9, *(unsigned short*)(p + 0x30)))    // phy_defense
        return false;
    if (!db->get_ushort(10, *(unsigned short*)(p + 0x32)))   // mag_attack
        return false;
    if (!db->get_ushort(11, *(unsigned short*)(p + 0x34)))   // mag_defense
        return false;
    if (!db->get_binary(12, p + 0x36, 8))         // element_resist
        return false;
    if (!db->get_binary(13, p + 0x3e, 0x22))      // spec_property
        return false;
    if (!db->get_int(14, *(int*)(p + 0x60)))      // inven_weight
        return false;
    if (!db->get_short(15, *(short*)(p + 0x64)))  // hp_regen
        return false;
    if (!db->get_short(16, *(short*)(p + 0x66)))  // mp_regen
        return false;
    if (!db->get_uint(17, *(unsigned int*)(p + 0x68)))    // move_speed
        return false;
    if (!db->get_ushort(18, *(unsigned short*)(p + 0x6c)))   // attack_speed
        return false;
    if (!db->get_ushort(19, *(unsigned short*)(p + 0x6e)))   // cast_speed
        return false;
    if (!db->get_short(20, *(short*)(p + 0x70)))  // hit_recovery
        return false;
    if (!db->get_short(21, *(short*)(p + 0x72)))  // jump
        return false;
    if (!db->get_int(22, *(int*)(p + 0x74)))      // charac_weight
        return false;
    return true;
}

// ============================================================================
// DB_LoadCharac::_UpdateSchoolCharac @0x08408c30
// school_member 更新：job(+0x30)/grow_type(+0x33)/lev(+0x31)，
// where school_id=+0x682 and m_id=NumberToString(accId,0) and charac_no=+0x4。
// 返回 exec(true) 结果。
// ============================================================================
bool DB_LoadCharac::_UpdateSchoolCharac(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0x10,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate school_member set job=%d,grow_type=%d,lev=%d where "
        "school_id=%d and m_id=%s and charac_no=%d",
        (int)(char)*(char*)SLC(sig, 0x30),        // job
        (int)(char)*(char*)SLC(sig, 0x33),        // grow_type
        (int)*(short*)SLC(sig, 0x31),             // lev
        *(int*)SLC(sig, 0x682),                   // school_id（m_schoolNo）
        NumberToString(sig->m_accId, 0),          // m_id
        sig->m_characNo);                         // charac_no
    return db->exec(true);
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
    *(unsigned char*)SLC(sig, 0x139) = useMaterialCnt;
    *(unsigned char*)SLC(sig, 0x138) = (giveRewardFlag != 0) ? 1 : 0;
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
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x233a);
    {
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(7);
        guard.operator*()->operator<<(slot);
        SIG_LOAD_CHARAC* out = guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC>();
        memcpy(out, sig, 0x703);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }                                                        // ~CStreamGuard

    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x2347, 0);
    trace("CharLastPlayTick char(%d) t(%d)", *(int*)SLC(sig, 0x04),
          *(int*)SLC(sig, 0x59));
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
    *(int*)SLC(sig, 0x08) = 0;                     // m_result

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
        *(int*)SLC(sig, 0x08) = line;              // m_result
    }

    _sendResult(userIdx, slot, sig);

    if (G_GameWorld()->IsSchoolPvPChannel() && *(int*)SLC(sig, 0x682) != 0)
    {
        _UpdateSchoolCharac(sig);
    }
    return true;
}
