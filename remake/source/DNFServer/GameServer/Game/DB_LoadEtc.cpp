// ============================================================================
// df_game_r 还原 —— DB_LoadEtc.cpp
// 目标：清零 /tmp/only_orig/DB_LoadEtc.txt 全部 40 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadEtc/<fn>.md（汇编为准）忠实转写。
// SIG_LOAD_ETC 子区偏移见 SigTypes.h 权威布局注记：
//   +0x340 PvpResultType(0x84) | +0xc9c0..+0x14a98 mixed 区（文档化偏移）
//   | +0x14a98 blueMarbleEnterCount | +0x14a99 SIG_ATTENDANCE_EVENT(0x11)
//   | +0x14aaa SIG_COMBO_SKILL 兼容区[0xb6]（尾至 +0x14b60 含 seria deco 残段）
// CHARAC_QUEST_SHOP_DATA 权威定义与区间 [0xd5a7,0xd651)=0xaa 双实例证据
// 见 DB_LoadEtc.h。
// ============================================================================

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "DB_LoadEtc.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"
#include "LogManager.h"        // logFormat
#include "CUserCharacInfo.h"    // stAuraAvatarOption::reset（ORIG W 0x844d616）
#include "CGuildServerProxy.h"  // CEventManager / CEventBase::IsEventing
#include "CDataManager.h"       // G_CDataManager / get_event_script_mng
#include "CEventScriptMng.h"    // EventClassify::CEventScriptMng/CEventEntity 完整类型
#include "CEnvironment.h"       // G_CEnvironment()->m_serverEnvir.m_fatigueTime(+0x37c)
#include "SyncScriptDeps.h"     // toTString(const std::string&)
#include "AdvanceAltarZip.h"    // uncompress_zip 权威声明
#include "TowerOfDespairMgr.h"  // TOD_Layer(unsigned short)
#include "WongWorkEvents_A.h"   // WongWork::EventCommon::LoadCharacEventItem
#include "DB_CreateCharac.h"    // dispatch 内 DB_CreateCharac 栈对象（W6_TailB 域）
#include "GameWorld.h"          // G_GameWorld()->IsPvPSkilTreeChannel

class CUser;   // CEventBase::IsEventing(CUser*) 形参前置
// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；照常调用，保持
// ORIG 全局 mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z20get_skill_table_nameb @0x083fbdc6 —— 调用方固定传 false。
char const* get_skill_table_name(bool use2nd);
// ORIG T _Z22getComboSkillTableName20ENUM_SKILL_TREE_KIND @0x0844a38b。
char const* getComboSkillTableName(ENUM_SKILL_TREE_KIND kind);
// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79
// （定义在 DB_Login.cpp:1967）。
bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap,
                              int recordSize);
// ORIG T _Z22getDailyInitializeTimeiii @0x086b1f5a（定义 GameUtils.cpp）。
int getDailyInitializeTime(int hour, int min, int sec);

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ORIG T GetCurrentResetBaseTime(long, int) @0x0810912a
// （定义 LinkClosureA_Misc.cpp）。
long GetCurrentResetBaseTime(long t, int hour);

namespace   // SIG_LOAD_ETC 文档化偏移访问 helper（同 CUser::GetEtcInfo 约定）
{
inline char* MIX(void* sig, unsigned int off)
{
    return reinterpret_cast<char*>(sig) + off;
}
template <typename T>
inline T& SGF(SIG_LOAD_ETC* sig, unsigned int off)
{
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(sig) + off);
}
}  // namespace

// ============================================================================
// DB_LoadEtc::DB_LoadEtc @0x08403b84（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x08403ba0 设 vptr→基类空 dtor→恒假 delete 分支；
// D0 0x08403bd0 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
DB_LoadEtc::DB_LoadEtc()
{
}

// ============================================================================
// ~DB_LoadEtc @0x08403ba0（D1/D2）/ deleting dtor @0x08403bd0（D0）
// ============================================================================
DB_LoadEtc::~DB_LoadEtc()
{
}

// ============================================================================
// DB_LoadEtc::GetUserRecord @0x084075c4：纯 ret 1（单指令函数体）。
// ============================================================================
int DB_LoadEtc::GetUserRecord(SIG_LOAD_ETC* sig)
{
    (void)sig;
    return 1;
}

// ============================================================================
// DB_LoadEtc::GetUserPvpBase @0x08407db4：纯 ret 1。
// ============================================================================
int DB_LoadEtc::GetUserPvpBase(SIG_LOAD_ETC* sig)
{
    (void)sig;
    return 1;
}

// ============================================================================
// DB_LoadEtc::GetUserPvp @0x08407dc2。
// 26 列 pvp_result 读取入 sig+0x340 PvpResultType；随后按本地当日零点重算
// m_nextResetTime(+0x68)，若晚于 last_play_time(col22) 清 +0x60；
// 若 pvp_grade_ext_update_time(col25)+86400 < 当前秒清 +0x5c。返 0=OK
// 1=无行 -1=DB/取值错误。
// ============================================================================
int DB_LoadEtc::GetUserPvp(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect win,lose,pvp_point,pvp_grade,avg_kill_count,"
                  "avg_buf_count,avg_debuf_count,avg_heal_count,"
                  "avg_counter_count,avg_back_atk_count,avg_union_hit_count,"
                  "avg_overkill_count,avg_combo_count,avg_aerial_count,"
                  "avg_attacked_count,avg_deal_damage,avg_technic,avg_style,"
                  "avg_hit_penalty,pvp_count,win_point,pvp_grade_ext,"
                  "unix_timestamp(last_play_time),play_count,play_time,"
                  "unix_timestamp(pvp_grade_ext_update_time) "
                  "from pvp_result where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
        return -1;
    if (!db->fetch())
        return 1;

    // sig+0x340 起为 PvpResultType（本 TU 内按 ORIG 偏移访问；权威字段布局
    // 见 PvpResultType.h，+0x0c/+0x10 两列 DB 不读）。
    static const unsigned int kPvpBase = 0x340;
    int v;
    for (int col = 0; col <= 2; ++col)              // win/lose/pvp_point
    {
        if (!db->get_int(col, v))
            return -1;
        SGF<int>(sig, kPvpBase + col * 4) = v;
    }
    if (!db->get_int(3, SGF<int>(sig, kPvpBase + 0x14)))   // pvp_grade（跳过 +0x0c/+0x10）
        return -1;
    for (int i = 0; i < 15; ++i)                    // col4..18 → avg[15](+0x18..+0x50)
    {
        if (!db->get_int(4 + i, SGF<int>(sig, kPvpBase + 0x18 + i * 4)))
            return -1;
    }
    if (!db->get_int(19, SGF<int>(sig, kPvpBase + 0x54)))  // pvp_count
        return -1;
    if (!db->get_int(20, SGF<int>(sig, kPvpBase + 0x58)))  // win_point
        return -1;
    if (!db->get_int(21, SGF<int>(sig, kPvpBase + 0x5c)))  // pvp_grade_ext
        return -1;

    int lastPlaySec = 0;
    if (!db->get_int(22, lastPlaySec))              // unix(last_play_time)→局部
        return -1;
    unsigned int u;
    if (!db->get_uint(23, u))                       // play_count→+0x64
        return -1;
    SGF<unsigned int>(sig, kPvpBase + 0x64) = u;
    if (!db->get_uint(24, u))                       // play_time→+0x60
        return -1;
    SGF<unsigned int>(sig, kPvpBase + 0x60) = u;

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    localtime_r(&now, &tmv);
    tmv.tm_sec = 0;
    tmv.tm_min = 0;
    tmv.tm_hour = 0;
    int nextReset = (int)mktime(&tmv);
    SGF<int>(sig, kPvpBase + 0x68) = nextReset;
    if (nextReset > lastPlaySec)
        SGF<int>(sig, kPvpBase + 0x60) = 0;

    int gradeExtUpdateSec = 0;
    if (!db->get_int(25, gradeExtUpdateSec))        // unix(pvp_grade_ext_update_time)
        return -1;
    if (gradeExtUpdateSec + 0x15180 < (int)GlobalData::s_systemTime_.getCurSec())
        SGF<int>(sig, kPvpBase + 0x5c) = 0;

    return 0;
}
// ============================================================================
// DB_LoadEtc::LoadLitmitItem @0x08405c60：sub $0x30 后直接 ret 1（如实空体）。
// ============================================================================
bool DB_LoadEtc::LoadLitmitItem(SIG_LOAD_ETC* sig)
{
    (void)sig;
    return true;
}

// ============================================================================
// DB_LoadEtc::dispatch @0x08403bee
// 调用序/分支/错误行号严格按报告汇编（0x14c6/0x14db/0x14e3/0x1566 等）。
// ============================================================================
bool DB_LoadEtc::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    // Stream::GetOutBuffer<SIG_LOAD_ETC>(param_3)（模板实例本 TU 隐式产出，
    // ORIG 对应 W 实例符号）。
    SIG_LOAD_ETC* sig = stream->GetOutBuffer<SIG_LOAD_ETC>();
    if (sig == 0)
    {
        return false;
    }

    GetUserRecord(sig);     // 返回值未被消费（ORIG call 后直接 continue）

    int pvp = GetUserPvp(sig);
    if (pvp == 0)
    {
        int base = GetUserPvpBase(sig);
        if (base == -1)
        {
            reinterpret_cast<PvpResultType*>(sig->m_pvpResult)->Clear();
        }
        else if (base == 1)
        {
            // *(undefined4*)(sig + 0x350) = -1 → PvpResultType+0x10（m_field10）。
            *reinterpret_cast<int*>(sig->m_pvpResult + 0x10) = -1;   // ORIG sig+0x350（PvpResultType+0x10）
        }
    }
    else if (pvp == 1)
    {
        {
            DB_CreateCharac createCharac;                 // C1 0x08400cc2
            createCharac.CreatePvP(sig->m_characNo);      // T 0x084026ec
        }                                                 // D1 0x08400cde + try 帧
    }
    else if (pvp == -1)
    {
        reinterpret_cast<PvpResultType*>(sig->m_pvpResult)->Clear();
    }

    if (!LoadCharacItemStat(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "virtual bool DB_LoadEtc::dispatch(int, int, Stream*)",
                              0x14c6, "DB_LoadETC ERROR : LoadCharacItemStat Fail");
        return false;
    }

    if (!LoadLitmitItem(sig))
    {
        cMyTrace trace("virtual bool DB_LoadEtc::dispatch(int, int, Stream*)",
                       0x14db, 0);
        trace("LoadLimitItem middle fetch <charac:%d>", sig->m_characNo);
        return false;
    }

    if (!GetUserSkill(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "virtual bool DB_LoadEtc::dispatch(int, int, Stream*)",
                              0x14e3, "DB_LoadETC ERROR : GetUserSkill Fail");
        sig->m_field8 = 1;
    }

    // GetRepeatEvent(0x71)->IsEventing(NULL)（虚表槽由 CEventBase 声明自然落位）
    if (GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
    {
        LoadConditionEventInfo(sig);
    }

    LoadProperDungeonClearCount(sig);
    LoadRedeemItem(sig);
    LoadPvPMissionList(sig);
    LoadTODUserState(sig);
    LoadVillageAttackInfo(sig);
    LoadAccountFirstLogin(sig, 0x191);
    LoadAccountFirstLogin(sig, 0x192);
    LoadAccountFirstLoginDaily(sig, 0x19c);
    LoadFairPvPScore(sig);

    if (!LoadItemMakingSkill(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "virtual bool DB_LoadEtc::dispatch(int, int, Stream*)",
                              0x1566, "DB_LoadETC ERROR : LoadItemMakingSkill Fail");
        return false;
    }
    if (!LoadQuestShop(sig))
    {
        return false;
    }

    LoadTitleBook(sig);
    LoadAchievement(sig);
    LoadOnceGiveAvengerTitle(sig);
    LoadAuraAvatarOption(sig);
    LoadCharacOption(userIdx, slot, sig);
    LoadBoosterGage(sig);
    LoadPcRoomPlayTimeInfo(sig);
    LoadUsedGiftFatigueInfo(sig);
    // attendance 视图实参 = SIG_LOAD_ETC+0x14a99（&gUnicodeBuffer 别名标注），
    // accId = *(uint*)sig。
    LoadAttendanceInfo(&sig->m_attendanceEvent, sig->m_accId);
    LoadComboSkillInfo(sig);
    LoadSeriaRoomAniDeco(sig);

    if (!SendResult(userIdx, slot, sig))
    {
        return false;
    }

    WongWork::EventCommon::LoadCharacEventItem(
        sig->m_accId /*(*(uint*)sig)*/, sig->m_characNo /*(*(uint*)(sig+4))*/, 0);
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadProperDungeonClearCount @0x084073aa
// ============================================================================
bool DB_LoadEtc::LoadProperDungeonClearCount(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect clear_cnt from event_dungeon_clear where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() != 1)
    {
        sig->m_properDungeonClearCount = 0;          // +0x40cc word 清零
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_short(0,
                       *reinterpret_cast<short*>(&SGF<unsigned short>(sig, 0x40cc))))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadConditionEventInfo @0x8407296？—— 报告头地址 0x08407296；
// 列 current_step(+0x40c8)/reward_step(+0x40ca)，缺行清零。
// ============================================================================
bool DB_LoadEtc::LoadConditionEventInfo(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect current_step, reward_step from event_conditionable_info"
                  " where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() != 1)
    {
        sig->m_curConditionEventStep = 0;            // +0x40c8
        sig->m_curConditionEventRewardStep = 0;      // +0x40ca
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_short(0,
                       *reinterpret_cast<short*>(&SGF<unsigned short>(sig, 0x40c8))))
    {
        return false;
    }
    if (!db->get_short(1,
                       *reinterpret_cast<short*>(&SGF<unsigned short>(sig, 0x40ca))))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship @0x08407482 —— 返回 LineFunc 错误码（0 成功）。
// npc 数据区 SIG_LOAD_ETC +0x3e6c(cnt)/+0x3e70(binary, ≤600 字节)。
// 错误码 0x1c80/0x1c84/0x1c87/0x1c8a/0x1c8d/0x1c90（见汇编各失败出口）。
// ============================================================================
int DB_LoadEtc::_loadNPCRelasionship(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect npc_cnt,npc_data from charac_npc where charac_no=%d",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return 0x1c80;
    }
    if (db->get_n_rows() != 1)
    {
        return 0x1c84;
    }
    if (!db->fetch())
    {
        return 0x1c87;
    }
    unsigned int npcCnt = 0;
    if (!db->get_uint(0, npcCnt))
    {
        return 0x1c8a;
    }
    SGF<unsigned int>(sig, 0x3e6c) = npcCnt;
    if (db->get_binary_length(1) != static_cast<int>(npcCnt) * 12)
    {
        return 0x1c8d;
    }
    if (!db->get_binary(1, MIX(sig, 0x3e70), 600))
    {
        return 0x1c90;
    }
    return 0;
}

// ============================================================================
// DB_LoadEtc::LoadRedeemItem @0x846a6a？—— 报告头地址 0x08406a6a；
// redeem_info blob → uncompress_zip → SIG_LOAD_ETC+0x40ce（m_expandData6 区）。
// ============================================================================
bool DB_LoadEtc::LoadRedeemItem(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect redeem_info from charac_inven_expand where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadRedeemItem(SIG_LOAD_ETC*)", 0x1b16, 5);
        trace("DB_LoadRedeemItemList::dispatch, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())
    {
        cMyTrace trace("bool DB_LoadEtc::LoadRedeemItem(SIG_LOAD_ETC*)", 0x1b20, 5);
        trace("DB_LoadRedeemItemList::dispatch, fetch() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    char blob[0x6c7];
    memset(blob, 0, sizeof(blob));
    unsigned int outLen = 0x6c7;
    if (!db->get_binary(0, blob, 0x6c7))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadRedeemItem(SIG_LOAD_ETC*)", 0x1b28, 5);
        trace("DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    int srcLen = db->get_binary_length(0);
    if (!uncompress_zip(MIX(sig, 0x40ce), outLen, blob,
                        static_cast<unsigned int>(srcLen)))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadPvPMissionList @0x08406cce —— fair_pvp_score.pvp_mission_info
// blob → SIG_LOAD_ETC+0xcdd5（type8 expand 区，cap 0x198）。n_rows==0 也返 0。
// ============================================================================
bool DB_LoadEtc::LoadPvPMissionList(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect pvp_mission_info from fair_pvp_score where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadPvPMissionList(SIG_LOAD_ETC*)", 0x1b3f, 5);
        trace("LoadPvPMissionList, exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())
    {
        cMyTrace trace("bool DB_LoadEtc::LoadPvPMissionList(SIG_LOAD_ETC*)", 0x1b49, 5);
        trace("LoadPvPMissionList, fetch() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }

    char blob[0x198];
    memset(blob, 0, sizeof(blob));
    unsigned int outLen = 0x198;
    if (!db->get_binary(0, blob, 0x198))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadPvPMissionList(SIG_LOAD_ETC*)", 0x1b51, 5);
        trace("DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    int srcLen = db->get_binary_length(0);
    if (!uncompress_zip(MIX(sig, 0xcdd5), outLen, blob,
                        static_cast<unsigned int>(srcLen)))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAchievement @0x08406422
// achievement zip → SIG_LOAD_ETC+0xb1a4（cap 0x1800），统计非零 0xc 条目计数入
// +0xc9a4，unix_timestamp(last_update_time) 入 +0xc9a8。
// ============================================================================
bool DB_LoadEtc::LoadAchievement(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect achievement, unix_timestamp(last_update_time) from"
                  " charac_achievement where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }

    char blob[0x1cd8];
    memset(blob, 0, sizeof(blob));
    if (!db->get_binary(0, blob, 0x1cd8))
    {
        return false;
    }
    unsigned int outLen = 0x1800;
    int srcLen = db->get_binary_length(0);
    if (!uncompress_zip(MIX(sig, 0xb1a4), outLen, blob,
                        static_cast<unsigned int>(srcLen)))
    {
        return false;
    }

    int count = 0;
    for (int i = 0; i <= 0x1ff; ++i)                       // i <= 0x1ff（cmpl 0x1ff setle）
    {
        if (SGF<int>(sig, 0xb1a4 + i * 12) == 0)
        {
            break;                                          // 计数停在首个零条目
        }
        ++count;
    }
    SGF<int>(sig, 0xc9a4) = count;

    unsigned int lastUpdate = 0;
    if (!db->get_uint(1, lastUpdate))
    {
        return false;
    }
    SGF<unsigned int>(sig, 0xc9a8) = lastUpdate;
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadOnceGiveAvengerTitle @0x08406600
// ============================================================================
bool DB_LoadEtc::LoadOnceGiveAvengerTitle(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    char* accStr = NumberToString(sig->m_accId, 0);
    db->set_query("seLect m_id from event_1107_avenger_plan where m_id=%s", accStr);
    if (!db->exec(true))
    {
        sig->m_flagPresentAvengerTitle = 1;                 // +0xc9ae
        return false;
    }
    sig->m_flagPresentAvengerTitle = (db->get_n_rows() != 0) ? 1 : 0;
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAuraAvatarOption @0x0840838e
// aura_avatar_option → stAuraAvatarOption 视图 @SIG_LOAD_ETC+0xd730。
// ============================================================================
bool DB_LoadEtc::LoadAuraAvatarOption(SIG_LOAD_ETC* sig)
{
    reinterpret_cast<stAuraAvatarOption*>(MIX(sig, 0xd730))->reset();

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect option_type, value_1 from aura_avatar_option"
                  " where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }
    for (int i = 0; i < nRows && db->fetch(); ++i)
    {
        int optionType = 0;
        int value = 0;
        if (!db->get_int(0, optionType))
        {
            return false;
        }
        if (!db->get_int(1, value))
        {
            return false;
        }
        if (-1 < optionType && optionType < 3)
        {
            // *(int*)(sig + (type + 0x35cc)*4) = value → +0xd730 + type*4。
            stAuraAvatarOption* opt =
                reinterpret_cast<stAuraAvatarOption*>(MIX(sig, 0xd730));
            (&opt->m_value0)[optionType] = value;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadCharacOption @0x084084e0
// options blob(+0xeac8,0x200)/best_clear_time blob(+0xec d8,24000)/负值钳位
// (+0xECDC..)/blueMarbleEnterCount(+0x14a98)/charac_inform_notice(+0x14a9a,16B)
// 及 best_clear_time 条目计数 +0xecd4。
// ============================================================================
bool DB_LoadEtc::LoadCharacOption(int userIdx, int slot, SIG_LOAD_ETC* sig)
{
    (void)userIdx;   // ORIG 仅透传第 3 参起作用（in_stack_00000010）
    (void)slot;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect options , best_clear_time, blue_marble_enter_count,"
                  " charac_inform_notice from charac_option where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }

    if (!db->get_binary(0, MIX(sig, 0xeac8), 0x200))         // col0 options
    {
        return false;
    }
    if (!get_compressed_blob_data(db, 1, MIX(sig, 0xecd8), 24000, 0))
    {                                                        // col1 best_clear_time
        cMyTrace trace("bool DB_LoadEtc::LoadCharacOption(int, int, SIG_LOAD_ETC*)",
                       0x1f5d, 5);
        trace("DB_LoadEtc::LoadCharacOption best_clear_time, get(%d) ERROR"
              " charac_no=%u", 2 /*local_18 已自增*/, sig->m_characNo);
        return false;
    }

    for (int i = 0; i <= 4; ++i)                             // base sig+0xC+(i+0x3b34)*4
    {
        unsigned int off = 0xc + (0x3b34 + i) * 4;           // = 0xECDC + i*4
        int* p = reinterpret_cast<int*>(MIX(sig, off));
        if (*p < 0)
        {
            *p = 0;
        }
    }

    SGF<unsigned char>(sig, 0x14a98) = 0;                    // 先置 0 再回填
    if (!db->get_ubyte(2, sig->m_blueMarbleEnterCount))
    {
        return false;
    }
    if (!db->get_str(3, MIX(sig, 0x14a9a), 16))
    {
        return false;
    }

    int count = 0;
    for (int i = 0; i <= 0x3e7; ++i)                         // 条目 stride 0x18@+0xecd8
    {
        unsigned int entry = 0xecd8 + i * 0x18;
        if (SGF<int>(sig, entry + 8) != 0)
        {
            ++count;
        }
    }
    SGF<int>(sig, 0xecd4) = count;
    return true;
}

// ============================================================================
// DB_LoadEtc::SendResult @0x08408766
// BigStreamPool::Acquire → guard<<userIdx<<slot→put_binary(sig,0x14b60) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
bool DB_LoadEtc::SendResult(int userIdx, int slot, SIG_LOAD_ETC* sig)
{
    Stream* stream = GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x1f92);
    {
        CStreamGuard guard(stream, true);
        guard << userIdx;
        guard << slot;
        guard->put_binary(sig, 0x14b60);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }                                                        // ~CStreamGuard
    return true;                                             // ORIG ebx=1 恒定
}

// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship 见上；LoadQuestShop/_LoadQuestShop 下。
// ============================================================================

// ============================================================================
// DB_LoadEtc::_LoadQuestShop @0x08404b72（static）
// get fail 行号序列：qp/init_count 前 2 次 → cMyTrace ret0（0x17ea/0x17f1）；
// 自 max_hp 起 throw int(line)：0x17f7..0x1811、questPiece 0x1816（跳过 0x1810）。
// catch 后 memset(param_2+0xc,0,0x9a) 并 logFormat 行 0x181f。
// ============================================================================
static void questShopThrow(int line)
{
    throw line;
}

bool DB_LoadEtc::_LoadQuestShop(unsigned int characNo, CHARAC_QUEST_SHOP_DATA* out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect qp, init_count, max_hp, max_mp, psy_attack, psy_defense,"
                  " mag_attack, mag_defence, move_speed, attack_speed, hp_regen,"
                  " mp_regen, all_element_resist, fire_element_resist,"
                  " water_element_resist, light_element_resist, dark_element_resist,"
                  " all_element_attack,fire_element_attack, water_element_attack,"
                  " light_element_attack, dark_element_attack, psy_critical,"
                  " mag_critical, good_hit , evasion , hit_recovery ,"
                  " separate_psy_mag_attack , quest_piece from charac_quest_shop"
                  " where charac_no=%u", characNo);
    try
    {
        if (!db->exec(true))
        {
            return false;
        }
        if (db->getAffectedRowCount() == 0)
        {
            return true;
        }
        if (!db->fetch())
        {
            return false;
        }

        // 列序/目标字段/失败抛出行号逐项对齐汇编（local_14 从 1 递增）。
        if (!db->get_uint(0, out->m_qp))
        {
            cMyTrace trace("static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                           " CHARAC_QUEST_SHOP_DATA*)", 0x17ea, 5);
            trace("DB_LoadEtc::LoadQuestShope, get(%d) ERROR charac_no=%u", 1,
                  characNo);
            return false;
        }
        if (!db->get_uint(1, out->m_initCount))
        {
            cMyTrace trace("static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                           " CHARAC_QUEST_SHOP_DATA*)", 0x17f1, 5);
            trace("DB_LoadEtc::LoadQuestShope, get(%d) ERROR charac_no=%u", 2,
                  characNo);
            return false;
        }

#       define QUESTSHOP_USHORT(col, field, line_)                             \
        do                                                                     \
        {                                                                      \
            if (!db->get_ushort(col, out->field))                              \
            {                                                                  \
                questShopThrow(line_);                                         \
            }                                                                  \
        } while (0)

        QUESTSHOP_USHORT(2, m_maxHp, 0x17f7);                   // max_hp           +0x26
        QUESTSHOP_USHORT(3, m_maxMp, 0x17f8);                   // max_mp           +0x6c
        QUESTSHOP_USHORT(4, m_psyAttack, 0x17f9);               // psy_attack       +0x0c
        QUESTSHOP_USHORT(5, m_psyDefense, 0x17fa);              // psy_defense      +0x12
        QUESTSHOP_USHORT(6, m_magAttack, 0x17fb);               // mag_attack       +0x0e
        QUESTSHOP_USHORT(7, m_magDefence, 0x17fc);              // mag_defence      +0x10
        QUESTSHOP_USHORT(8, m_moveSpeed, 0x17fd);               // move_speed       +0x22
        QUESTSHOP_USHORT(9, m_attackSpeed, 0x17fe);             // attack_speed     +0x20
        QUESTSHOP_USHORT(10, m_hpRegen, 0x17ff);                // hp_regen         +0x28
        QUESTSHOP_USHORT(11, m_mpRegen, 0x1800);                // mp_regen         +0x46
        QUESTSHOP_USHORT(12, m_allElementResist, 0x1801);       // +0x3c
        QUESTSHOP_USHORT(13, m_fireElementResist, 0x1802);      // +0x34
        QUESTSHOP_USHORT(14, m_waterElementResist, 0x1803);     // +0x36
        QUESTSHOP_USHORT(15, m_lightElementResist, 0x1804);     // +0x3a
        QUESTSHOP_USHORT(16, m_darkElementResist, 0x1805);      // +0x38
        QUESTSHOP_USHORT(17, m_allElementAttack, 0x1806);       // +0x68
        QUESTSHOP_USHORT(18, m_fireElementAttack, 0x1807);      // +0x60
        QUESTSHOP_USHORT(19, m_waterElementAttack, 0x1808);     // +0x62
        QUESTSHOP_USHORT(20, m_lightElementAttack, 0x1809);     // +0x66
        QUESTSHOP_USHORT(21, m_darkElementAttack, 0x180a);      // +0x64
        QUESTSHOP_USHORT(22, m_psyCritical, 0x180b);            // +0x2a
        QUESTSHOP_USHORT(23, m_magCritical, 0x180c);            // +0x2c
        QUESTSHOP_USHORT(24, m_goodHit, 0x180d);                // +0x4e
        QUESTSHOP_USHORT(25, m_evasion, 0x180e);                // +0x42
        QUESTSHOP_USHORT(26, m_hitRecovery, 0x180f);            // +0x48
        QUESTSHOP_USHORT(27, m_separatePsyMagAttack, 0x1811);   // +0x76（跳过 0x1810）

#       undef QUESTSHOP_USHORT

        if (!db->get_uint(28, out->m_questPiece))               // quest_piece +0xa6
        {
            questShopThrow(0x1816);
        }
    }
    catch (int e)
    {
        memset(reinterpret_cast<char*>(out) + 0xc, 0, 0x9a);
        LogManager::logFormat(1, "DBThread.cpp",
                              "static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                              " CHARAC_QUEST_SHOP_DATA*)",
                              0x181f,
                              "DB_LoadEtc::LoadQuestShop, get(%d) ERROR charac_no=%u",
                              e /*ORIG local_14 递增行号*/, characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadQuestShop @0x08404a68 —— 主/tag 双实例装载。
// 主区 = SIG_LOAD_ETC+0xd5a7，tag 区 = +0xd651（tag_charac_no=+0xc 非零时装载），
// 双区齐备后 +0xd6fb 置 1。错误走 cMyTrace 0x16c4/0x16d5。
// ============================================================================
bool DB_LoadEtc::LoadQuestShop(SIG_LOAD_ETC* sig)
{
    if (!_LoadQuestShop(sig->m_characNo,
                        reinterpret_cast<CHARAC_QUEST_SHOP_DATA*>(MIX(sig, 0xd5a7))))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadQuestShop(SIG_LOAD_ETC*)", 0x16c4, 5);
        trace("DB_LoadInventory::_LoadQuestShop ERROR::charac_no(%d)",
              sig->m_characNo);
        return false;
    }

    if (sig->m_fieldC != 0)
    {
        if (!_LoadQuestShop(sig->m_fieldC,
                            reinterpret_cast<CHARAC_QUEST_SHOP_DATA*>(
                                MIX(sig, 0xd651))))
        {
            cMyTrace trace("bool DB_LoadEtc::LoadQuestShop(SIG_LOAD_ETC*)", 0x16d5, 5);
            trace("DB_LoadInventory::_LoadQuestShop ERROR::tag_charac_no(%d)",
                  sig->m_fieldC);
            return false;
        }
        SGF<unsigned char>(sig, 0xd6fb) = 1;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadTitleBook @0x08406092 —— 4 个压缩列分别解压到
// +0x466a(cap0x10ae) / +0x5718(0x2fa8) / +0x86c0(0x1c98) / +0xa358(0xe4c)。
// ============================================================================
bool DB_LoadEtc::LoadTitleBook(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect general_section, specific_section, despair, event from"
                  " charac_titlebook where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadTitleBook(SIG_LOAD_ETC*)", 0x19c0, 5);
        trace("LoadTitleBook(), exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0 || !db->fetch())
    {
        return false;
    }

    char blob[0x397c];
    struct ColDef
    {
        int capMax;      // get_binary size 上限（get 前 memset 长度=capMax）
        unsigned int dstOff;   // 解压目的偏移
        unsigned int dstCap;   // 解压容量初值
    };
    const ColDef cols[4] =
    {
        { 0x1410, 0x466a, 0x10ae },
        { 0x393c, 0x5718, 0x2fa8 },
        { 0x225c, 0x86c0, 0x1c98 },
        { 0x1134, 0xa358, 0x0e4c },
    };
    for (int col = 0; col < 4; ++col)
    {
        memset(blob, 0, cols[col].capMax);
        if (!db->get_binary(col, blob, cols[col].capMax))
        {
            return false;
        }
        unsigned int outLen = cols[col].dstCap;
        int srcLen = db->get_binary_length(col);
        if (!uncompress_zip(MIX(sig, cols[col].dstOff), outLen, blob,
                            static_cast<unsigned int>(srcLen)))
        {
            return false;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadComboSkillInfo @0x0844a3b2 —— combo 区 job==9 才拉双树。
// ============================================================================
void DB_LoadEtc::LoadComboSkillInfo(SIG_LOAD_ETC* sig)
{
    SIG_COMBO_SKILL* cs = reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill);
    if (cs->m_job != 9)                                      // *(int*)(sig+0x14ab0)==9
    {
        return;
    }
    if (!LoadComboSkill(sig, (ENUM_SKILL_TREE_KIND)0))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "void DB_LoadEtc::LoadComboSkillInfo(SIG_LOAD_ETC*)",
                              0xbab4, "DB_LoadETC ERROR : LoadComboSkill Fail");
        sig->m_field8 = 1;
    }
    if (!LoadComboSkill(sig, (ENUM_SKILL_TREE_KIND)1))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "void DB_LoadEtc::LoadComboSkillInfo(SIG_LOAD_ETC*)",
                              0xbaba, "DB_LoadETC ERROR : LoadComboSkill Second Fail");
        sig->m_field8 = 1;
    }
}


// ============================================================================
// DB_LoadEtc::LoadComboSkill @0x0844a47a —— 单树连招装载（kind∈{0,1}）。
// ============================================================================
bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC* sig, ENUM_SKILL_TREE_KIND kind)
{
    unsigned int kindVal = (unsigned int)kind;
    if (!(kindVal == 0 || kindVal == 1))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                       " ENUM_SKILL_TREE_KIND)", 0xbac4, 5);
        trace("DB_LoadEtc::LoadComboSkill kind(%d) error characNo(%u)", kind,
              sig->m_characNo);
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        return false;
    }
    db->set_query("seLect combo_idx, value1, value2, value3, value4, value5,"
                  " value6 from %s where charac_no=%u",
                  getComboSkillTableName(kind), sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                       " ENUM_SKILL_TREE_KIND)", 0xbad4, 5);
        trace("DB_LoadEtc::LoadComboSkill, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }

    // 树条目计数写回：kind0→+0x14aae（tree1Count）、kind1→+0x14aaf。
    SIG_COMBO_SKILL* cs = reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill);
    comoboSkillData* dst =
        (kindVal == 0) ? cs->m_tree1Data : cs->m_tree2Data;   // +0x14ab4 / +0x14b08
    if (kindVal == 0)
    {
        cs->m_tree1Count = (unsigned char)nRows;
    }
    else
    {
        cs->m_tree2Count = (unsigned char)nRows;
    }
    if (!getComboSkill(db, sig->m_accId, nRows, dst))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                       " ENUM_SKILL_TREE_KIND)",
                       (kindVal == 0) ? 0xbae7 : 0xbaef, 5);
        trace("DB_LoadEtc::LoadComboSkill, getComboSkill ERROR charac_no=%u",
              sig->m_characNo);
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::getComboSkill @0x0844a704 —— 每行 7 个 short 填一个 comoboSkillData，
// 行数上限 min(count,6)；成功返回 1、任何 get 失败行号错误路径返回 0。
// ============================================================================
bool DB_LoadEtc::getComboSkill(MySQL* db, unsigned int accId, int count,
                               comoboSkillData* out)
{
    if (db == 0 || out == 0)
    {
        return false;
    }
    for (int row = 0;; ++row)
    {
        if (!(row < count && row < 6))
        {
            return true;
        }
        if (!db->fetch())
        {
            cMyTrace trace("bool DB_LoadEtc::getComboSkill(MySQL*, unsigned int,"
                           " int, comoboSkillData*)", 0xbb01, 5);
            trace("DB_LoadEtc::getComboSkill fetch, exec() ERROR charac_no=%u",
                  accId);
            return false;
        }
        comoboSkillData* slot =
            reinterpret_cast<comoboSkillData*>(reinterpret_cast<char*>(out)
                                               + row * 0xe);
        int col = 1;
        if (!db->get_short(0, slot->m_comboSkillIndex))       // col0 → +row*0xe
        {
            cMyTrace trace("bool DB_LoadEtc::getComboSkill(MySQL*, unsigned int,"
                           " int, comoboSkillData*)", 0xbb0a, 5);
            trace("DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u", col,
                  accId);
            return false;
        }
        for (int k = 0; k < 6; ++k)
        {
            if (!db->get_short(col++, slot->m_comboSkillList[k]))   // 每值一行号表
            {
                cMyTrace trace("bool DB_LoadEtc::getComboSkill(MySQL*,"
                               " unsigned int, int, comoboSkillData*)", 0xbb14, 5);
                trace("DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u",
                      col, accId);
                return false;
            }
        }
    }
}

// ============================================================================
// DB_LoadEtc::GetUserSkill @0x084075d2 —— 主 skill 行 + tag 双槽位 blob。
// 列序 12 列，目标偏移照汇编；n_rows==0 且 PvP 技能树频道时补插一行。
// 错误行号 0x1d50/0x1d5d/0x1d69/0x1d71/0x1d98/0x1dac/0x1db2/0x1dea/0x1df1。
// ============================================================================
bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect remain_sp,skill_slot,remain_sp_2nd, skill_slot_2nd,"
                  " remain_sfp_1st, remain_sfp_2nd, skill_command, skill_slot_lethe,"
                  " lethe_flag, skill_slot_lethe_2nd, lethe_flag_2nd,script_version"
                  " from %s where charac_no=%u",
                  get_skill_table_name(false), sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)", 0x1d50, 5);
        trace("DB_LoadSkill::GetUserSkill, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    if (db->get_n_rows() == 0 && G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG：缺行 + PvP 技能树频道 → inSert 占位行并置 0xcdd4 标志。
        db->set_query("inSert into %s (charac_no) values(%u)",
                      get_skill_table_name(false), sig->m_characNo);
        if (!db->exec(true))
        {
            cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)",
                           0x1d5d, 5);
            trace("DB_LoadSkill::GetUserSkill insert, exec() ERROR charac_no=%u",
                  sig->m_characNo);
            return false;
        }
        SGF<unsigned char>(sig, 0xcdd4) = 1;
        return true;
    }

    if (!db->fetch())
    {
        cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)", 0x1d69, 5);
        trace("DB_LoadSkill::GetUserSkill, fetch() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    int col = 1;                                             // local_14 从 1 起
    unsigned int spVals[4];
    int letheFlags[2] = { 0, 0 };

    if (!db->get_uint(0, SGF<unsigned int>(sig, 0xc9c8)))    // col0 remain_sp
    {
        cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)", 0x1d71, 5);
        trace("DB_LoadSkill::GetUserSkill, get(0) ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    ++col;
    if (!get_compressed_blob_data(db, col++, MIX(sig, 0xc9cc), 0x198, 2))
    {                                                        // col1 skill_slot
        return false;
    }
    if (!db->get_uint(col++, spVals[0]))                     // col2? → get(4)行号
    {                                                        // remain_sp_2nd→0xcb64
        cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)", 0x1d98, 5);
        trace("DB_LoadSkill::GetUserSkill, get(4) ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    SGF<unsigned int>(sig, 0xcb64) = spVals[0];
    if (!get_compressed_blob_data(db, col++, MIX(sig, 0xcb68), 0x198, 2))
    {                                                        // skill_slot_2nd
        return false;
    }
    if (!db->get_uint(col++, spVals[1]))                     // remain_sfp_1st→0xcd00
    {
        cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)", 0x1dac, 5);
        trace("DB_LoadSkill::GetUserSkill, get(7) ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    SGF<unsigned int>(sig, 0xcd00) = spVals[1];
    if (!db->get_uint(col++, spVals[2]))                     // remain_sfp_2nd→0xcd04
    {
        cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)", 0x1db2, 5);
        trace("DB_LoadSkill::GetUserSkill, get(8) ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    SGF<unsigned int>(sig, 0xcd04) = spVals[2];
    (void)spVals[3];
    if (!get_compressed_blob_data(db, col++, MIX(sig, 0xcd08), 200, 4))
    {                                                        // skill_command
        return false;
    }
    if (!get_compressed_blob_data(db, col++, MIX(sig, 0x29fd), 0x198, 2))
    {                                                        // skill_slot_lethe
        return false;
    }
    if (!db->get_int(col++, letheFlags[0]))                  // lethe_flag(int)
    {
        return false;
    }
    if (!get_compressed_blob_data(db, col++, MIX(sig, 0x2b97), 0x198, 2))
    {                                                        // skill_slot_lethe_2nd
        return false;
    }
    if (!db->get_int(col++, letheFlags[1]))                  // lethe_flag_2nd(int)
    {
        return false;
    }
    unsigned char scriptVersion = 0;
    if (!db->get_ubyte(col++, scriptVersion))                // script_version
    {
        return false;
    }
    SGF<unsigned char>(sig, 0xcdd1) = scriptVersion;         // ORIG get_ubyte→+0xcdd1


    // lethe 标志低位字节落位（ORIG movzbl/mov dl）。
    SGF<unsigned char>(sig, 0x2b95) = (unsigned char)letheFlags[0];
    SGF<unsigned char>(sig, 0x29fc) = (unsigned char)letheFlags[0];
    SGF<unsigned char>(sig, 0x2d2f) = (unsigned char)letheFlags[1];
    SGF<unsigned char>(sig, 0x2b96) = (unsigned char)letheFlags[1];

    if (sig->m_fieldC != 0)
    {
        // tagCharacNo 存在 → 拉主/tag 双槽位 blob。
        db->set_query("seLect skill_slot, skill_slot_2nd from skill where"
                      " charac_no=%u", sig->m_fieldC);
        if (!db->exec(true))
        {
            cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)",
                           0x1dea, 5);
            trace("DB_LoadSkill::GetUserSkill, exec() ERROR tagCharacNo=%u",
                  sig->m_fieldC);
            return false;
        }
        if (!db->fetch())
        {
            cMyTrace trace("bool DB_LoadEtc::GetUserSkill(SIG_LOAD_ETC*)",
                           0x1df1, 5);
            trace("DB_LoadSkill::GetUserSkill, fetch() ERROR tagCharacNo=%u",
                  sig->m_fieldC);
            return false;
        }
        if (!get_compressed_blob_data(db, 0, MIX(sig, 0x10), 0x198, 2))
        {
            return false;
        }
        if (!get_compressed_blob_data(db, 1, MIX(sig, 0x1a8), 0x198, 2))
        {
            return false;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadCharacItemStat @0x08445660 —— cooltime/effect blob 双列 +
// check_flag 压缩列（cap 5000）入 +0xd740。错行号 0xac8a/0xac91/0xac98。
// ============================================================================
bool DB_LoadEtc::LoadCharacItemStat(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect cooltime_item, effect_item, check_flag from"
                  " charac_item_stat where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }

    if (!db->get_binary(0, MIX(sig, 0x230c), 0x194))         // cooltime_item
    {
        cMyTrace trace("bool DB_LoadEtc::LoadCharacItemStat(SIG_LOAD_ETC*)",
                       0xac8a, 5);
        trace("DB_LoadEtc::LoadCharacItemStat cooltime, get(%d) ERROR"
              " charac_no=%u", 1 /*local_10*/, sig->m_characNo);
        return false;
    }
    if (!db->get_binary(1, MIX(sig, 0x24a4), 0x194))         // effect_item
    {
        cMyTrace trace("bool DB_LoadEtc::LoadCharacItemStat(SIG_LOAD_ETC*)",
                       0xac91, 5);
        trace("DB_LoadEtc::LoadCharacItemStat effect_item, get(%d) ERROR"
              " charac_no=%u", 2 /*local_10 已自增*/, sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, 2, MIX(sig, 0xd740), 5000, 0))
    {                                                        // check_flag
        cMyTrace trace("bool DB_LoadEtc::LoadCharacItemStat(SIG_LOAD_ETC*)",
                       0xac98, 5);
        trace("DB_LoadEtc::LoadCharacItemStat item_dictionary, get(%d) ERROR"
              " charac_no=%u", 3 /*local_10 已自增*/, sig->m_characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadItemMakingSkill @0x084040b4 —— 11 列 short 循环装载，
// 目标基址 sig+0x10+(col+0x6b78)*2（=0xD700+col*2）。错行号 0x15dd/0x15e8/0x15f2。
// ============================================================================
bool DB_LoadEtc::LoadItemMakingSkill(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect weapon, cloth, leather, light_armor, heavy_armor, plate,"
                  " amulet, wrist, ring, support, magic_stone from"
                  " item_making_skill_info where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadItemMakingSkill(SIG_LOAD_ETC*)",
                       0x15dd, 5);
        trace("DB_LoadEtc::LoadItemMakingSkill, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        cMyTrace trace("bool DB_LoadEtc::LoadItemMakingSkill(SIG_LOAD_ETC*)",
                       0x15e8, 5);
        trace("DB_LoadEtc::LoadItemMakingSkill fetch, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    for (int col = 0; col <= 0xa; ++col)                     // cmpl $0xa setle → ≤10
    {
        // ORIG 地址式：sig + 0x10 + (col + 0x6b78)*2。
        unsigned int off = 0x10 + (unsigned int)(col + 0x6b78) * 2;
        if (!db->get_short(col,
                           *reinterpret_cast<short*>(&SGF<unsigned short>(sig,
                                                                          off))))
        {
            cMyTrace trace("bool DB_LoadEtc::LoadItemMakingSkill(SIG_LOAD_ETC*)",
                           0x15f2, 5);
            trace("DB_LoadEtc::LoadItemMakingSkill get(%d) ERROR charac_no=%u",
                  col, sig->m_characNo);
            return false;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadTODUserState @0x08406eec —— charac_tower_despair 六列；
// 前置 exec/fetch/AffectedRowCount==1 任一失败均如实 ret 1；列失败走 cMyTrace 0。
// col2 last_clear_layer 经 TOD_Layer(unsigned short) 构造后取其 m_layer 落位。
// ============================================================================
bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect unix_timestamp(first_layer_start_date),"
                  "today_enter_count,last_clear_layer,enter_count_by_week,"
                  "unix_timestamp(m_date),unix_timestamp(last_clear_date) from"
                  " charac_tower_despair where charac_no = %u", sig->m_characNo);
    if (!db->exec(true))
    {
        return true;
    }
    if (!db->fetch())
    {
        return true;
    }
    if (!(db->getAffectedRowCount() == 1))                   // longlong == 1
    {
        return true;
    }

    int col = 1;
    if (!db->get_int(0, SGF<int>(sig, 0xd71c)))
    {                                                        // first_layer_start_date
        cMyTrace trace("bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC*)",
                       0x1b77, 5);
        trace("TOD : select from charac_tower_despair charac_no=%u",
              sig->m_characNo);
        return false;
    }
    ++col;
    if (!db->get_int(col++, SGF<int>(sig, 0xd720)))
    {                                                        // today_enter_count
        cMyTrace trace("bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC*)",
                       0x1b7f, 5);
        trace("TOD select from charac_tower_despair charac_no=%u",
              sig->m_characNo);
        return false;
    }
    unsigned short layerVal = 0;
    if (!db->get_ushort(col++, layerVal))                    // last_clear_layer(ushort)
    {
        cMyTrace trace("bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC*)",
                       0x1b88, 5);
        trace("[@TOD] select from charac_tower_despair charac_no=%d",
              sig->m_characNo);
        return false;
    }
    TOD_Layer layer(layerVal);                               // ORIG TOD_Layer C1(T)@0x085fe7b4
    SGF<unsigned short>(sig, 0xd724) = layer.m_layer;
    if (!db->get_short(col++,
                       *reinterpret_cast<short*>(&SGF<unsigned short>(sig,
                                                                      0xd726))))
    {                                                        // enter_count_by_week
        cMyTrace trace("bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC*)",
                       0x1b92, 5);
        trace("[@TOD] select from charac_tower_despair charac_no=%d",
              sig->m_characNo);
        return false;
    }
    if (!db->get_int(col++, SGF<int>(sig, 0xd728)))
    {                                                        // m_date
        cMyTrace trace("bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC*)",
                       0x1b9a, 5);
        trace("[@TOD] select from charac_tower_despair charac_no=%d",
              sig->m_characNo);
        return false;
    }
    if (!db->get_int(col, SGF<int>(sig, 0xd72c)))
    {                                                        // last_clear_date
        cMyTrace trace("bool DB_LoadEtc::LoadTODUserState(SIG_LOAD_ETC*)",
                       0x1ba2, 5);
        trace("[@TOD] select from charac_tower_despair charac_no=%d",
              sig->m_characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadVillageAttackInfo @0x0843f038 —— 按重置基准日统计当日
// attack_count/revenge_dungeon（+0xc9ac/+0xc9ad）；无记录两字节清零且 ret 0。
// ============================================================================
bool DB_LoadEtc::LoadVillageAttackInfo(SIG_LOAD_ETC* sig)
{
    if (sig->m_characNo == 0)
    {
        return false;
    }

    long cur = GlobalData::s_systemTime_.getCurSec();        // ORIG localtime 单线程域
    struct tm* ptm = localtime(&cur);
    int fatigueHour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // +0x37c
    if (ptm->tm_hour < fatigueHour)
    {
        ptm = 0;
        cur -= 0x15180;
        ptm = localtime(&cur);
    }
    time_t baseDay = mktime(ptm);
    (void)baseDay;

    char occDate[12];
    memset(occDate, 0, sizeof(occDate));
    snprintf(occDate, sizeof(occDate), "%04d-%02d-%02d",
             ptm->tm_year + 0x76c, ptm->tm_mon + 1, ptm->tm_mday);

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
    {
        return false;
    }
    db->set_query("seLect attack_count, revenge_dungeon from"
                  " village_attack_dungeon \t\t\t\t\t\t   where occ_date = '%s'"
                  " and charac_no = %u", occDate, sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_curVillageAttackCount = 0;                    // +0xc9ac
        sig->m_curRevengeDungeonCount = 0;                   // +0xc9ad
        return false;
    }
    int fetched = db->fetch() ? 1 : 0;                       // fetch 返回值 &0xff
    if (fetched == 0)
    {
        return false;
    }
    if (!db->get_ubyte(0, sig->m_curVillageAttackCount))
    {
        return false;
    }
    if (!db->get_ubyte(1, sig->m_curRevengeDungeonCount))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAccountFirstLogin @0x08405c6e —— 首登事件位图装载。
// bitset 位操作映射 std::bitset<32>::set/reset(pos, true) ORIG 外联符号
// （084506e4 set / 0845071e reset）。
// ============================================================================
bool DB_LoadEtc::LoadAccountFirstLogin(SIG_LOAD_ETC* sig, int eventCode)
{
    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    if (!evMng->is_eventing(eventCode))
    {
        return false;
    }
    EventClassify::CEventEntity* entity = evMng->get_event_entity(eventCode);
    if (entity == 0)
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    char* accStr = NumberToString(sig->m_accId, 0);
    // ORIG vtable+0x20 虚调用 get_db_table_name → &m_dbTableName（std::string 视图）
    // 经 toTString(std::string const&) 取表名。
    const char* tableName =
        toTString(*reinterpret_cast<std::string*>(entity->get_db_table_name()));
    db->set_query("seLect m_id from %s where m_id=%s", tableName, accStr);

    unsigned int flagPos =
        (unsigned int)evMng->get_convert_to_user_confirmflag(eventCode);
    if (!db->exec(true))
    {
        sig->m_bitset |= (1u << flagPos);                    // set(pos,true)
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_bitset &= ~(1u << flagPos);                   // reset(pos)
    }
    else
    {
        sig->m_bitset |= (1u << flagPos);                    // set(pos,true)
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAccountFirstLoginDaily @0x08405e0a —— 日频首登事件。
// ============================================================================
bool DB_LoadEtc::LoadAccountFirstLoginDaily(SIG_LOAD_ETC* sig, int eventCode)
{
    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    if (!evMng->is_eventing(eventCode))
    {
        return false;
    }
    EventClassify::CEventEntity* entity = evMng->get_event_entity(eventCode);
    if (entity == 0)
    {
        return false;
    }

    int today = evMng->get_today_time();
    if (today < entity->get_event_start_time()
        || today > entity->get_event_end_time())
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    time_t nowT = time(0);
    struct tm curTm;
    localtime_r(&nowT, &curTm);
    struct tm resetTm;
    memcpy(&resetTm, &curTm, sizeof(resetTm));               // ORIG 字段复制视图
    resetTm.tm_hour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;       // ORIG tm_hour=+0x37c
    resetTm.tm_min = 0;
    resetTm.tm_sec = 0;
    long baseSec = mktime(&resetTm);
    if (curTm.tm_hour < resetTm.tm_hour)
    {
        baseSec -= 0x15180;
    }

    const char* tableName =
        toTString(*reinterpret_cast<std::string*>(entity->get_db_table_name()));
    db->set_query("seLect m_id from %s where m_id=%u and occ_date >"
                  " FROM_UNIXTIME(%d)", tableName, sig->m_accId, baseSec);

    unsigned int flagPos =
        (unsigned int)evMng->get_convert_to_user_confirmflag(eventCode);
    if (!db->exec(true))
    {
        sig->m_bitset |= (1u << flagPos);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_bitset &= ~(1u << flagPos);
    }
    else
    {
        sig->m_bitset |= (1u << flagPos);
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadFairPvPScore @0x08404290 —— fair_pvp_score 14 列；
// n_rows==0 时补插并返回；give_item 压缩列（cap 0x650）→ +0xcf4b；
// daily 重置比较（last_play_time < getDailyInitializeTime(6,0,0) → +0xd59b=0）。
// ============================================================================
bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect private_win,private_lose,private_draw,relay_battle_win,"
                  " relay_battle_lose, relay_battle_draw,relay_battle_2kill,"
                  " relay_battle_3kill, successive_win, max_successive_win,"
                  " job_score,daily_play_count, unix_timestamp(last_play_time),"
                  " give_item from fair_pvp_score where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                       0x1618, 5);
        trace("DB_LoadSkill::LoadFairPvPScore, exec() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    int col = 1;
    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into fair_pvp_score (charac_no) values(%u)",
                      sig->m_characNo);
        if (!db->exec(true))
        {
            cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                           0x1625, 5);
            trace("DB_LoadEtc::LoadFairPvPScore insert, exec() ERROR"
                  " charac_no=%u", sig->m_characNo);
            return false;
        }
        return true;
    }

    if (!db->fetch())
    {
        cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                       0x162f, 5);
        trace("DB_LoadEtc::LoadFairPvPScore, fetch() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    // 前 6 列按 i∈[0,1] 三连 uint 结构装载（stride 0xc，基址 +0xcf23）。
    for (int i = 0; i <= 1; ++i)
    {
        unsigned int base = 0xcf23 + i * 12;
        for (int k = 0; k < 3; ++k)
        {
            const int lineNo =
                (k == 0) ? 0x1638 : (k == 1) ? 0x163f : 0x1646;
            if (!db->get_uint(col++,
                              SGF<unsigned int>(sig, base + (unsigned)k * 4)))
            {
                cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                               lineNo, 5);
                trace("DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                      col, sig->m_characNo);
                return false;
            }
        }
    }

    static const struct
    {
        unsigned int off;
        int line;
    } plainCols[] =
    {
        { 0xcf3b, 0x1667 },   // successive_win
        { 0xcf3f, 0x166e },   // max_successive_win
        { 0xcf43, 0x1675 },   // job_score
        { 0xcf47, 0x167c },   // daily_play_count
    };
    for (int n = 0; n < 4; ++n)
    {
        if (!db->get_uint(col++, SGF<unsigned int>(sig, plainCols[n].off)))
        {
            cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                           plainCols[n].line, 5);
            trace("DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                  col, sig->m_characNo);
            return false;
        }
    }

    if (!get_compressed_blob_data(db, col++, MIX(sig, 0xcf4b), 0x650, 0))
    {                                                        // give_item
        return false;
    }
    if (!db->get_uint(col++, SGF<unsigned int>(sig, 0xd59b)))
    {                                                        // unix_timestamp(last_play_time)
        cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                       0x1689, 5);
        trace("DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
              col, sig->m_characNo);
        return false;
    }
    int lastPlay = 0;
    if (!db->get_int(col++, lastPlay))                       // give_item 后整型重读？
    {
        // ORIG 本列实为 get_int(行号 0x1690)；占位目标与下方一致。
        cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                       0x1690, 5);
        trace("DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
              col, sig->m_characNo);
        return false;
    }
    SGF<int>(sig, 0xd59f) = lastPlay;                        // 实际报告写点 +0xd59f
    if (SGF<int>(sig, 0xd59f) < getDailyInitializeTime(6, 0, 0))
    {
        SGF<unsigned int>(sig, 0xd59b) = 0;
    }
    unsigned char giveFlag = 0;
    if (!db->get_ubyte(col++, giveFlag))                     // 末列 → +0xd5a3
    {
        cMyTrace trace("bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC*)",
                       0x16a5, 5);
        trace("DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
              col, sig->m_characNo);
        return false;
    }
    SGF<unsigned char>(sig, 0xd5a3) = giveFlag;
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadBoosterGage @0x08446d9e —— member_booster_gage gage 单列
// 入 +0xeccc（accId 维度）。错行号 0xafc4。
// ============================================================================
bool DB_LoadEtc::LoadBoosterGage(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect gage from member_booster_gage where m_id=%u",
                  sig->m_accId);
    bool ok = db->exec(true);                                // local_19 保存复用
    if (!ok)
    {
        return false;
    }
    if (db->get_n_rows() != 1 || !ok)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_int(0, SGF<int>(sig, 0xeccc)))
    {
        cMyTrace trace("bool DB_LoadEtc::LoadBoosterGage(SIG_LOAD_ETC*)",
                       0xafc4, 5);
        trace("DB_LoadEtc::LoadBoosterGage, get(%d) ERROR m_id=%u", 1, sig->m_accId);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadPcRoomPlayTimeInfo @0x084066ae —— 事件关闭时如实 ret 1；
// play_time(+0xc9b3)/reward_cnt(+0xc9b7)。无记录 ret 1。
// ============================================================================
bool DB_LoadEtc::LoadPcRoomPlayTimeInfo(SIG_LOAD_ETC* sig)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5c);
    if (!ev->IsEventing((CUser*)0))                          // ORIG else → ret 1
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    char* accStr = NumberToString(sig->m_accId, 0);
    db->set_query("seLect play_time, reward_cnt from event_1204_pcroom_play_time"
                  " where m_id = %s and occ_date > curdate()", accStr);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    unsigned int playTime = 0;
    unsigned char rewardCnt = 0;
    if (!db->get_uint(0, playTime))
    {
        return false;
    }
    if (!db->get_ubyte(1, rewardCnt))
    {
        return false;
    }
    sig->m_totalPcRoomPlayTime = playTime;                   // +0xc9b3
    sig->m_pcRoomPlayTimeRewardCnt = rewardCnt;              // +0xc9b7
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadUsedGiftFatigueInfo @0x08406810 —— CUsedFatigueGiveItem 事件
// 驱动的 fatigue_quantity 装载（账号表 idx9 / 角色表 idx2 双路径）。
// ============================================================================
bool DB_LoadEtc::LoadUsedGiftFatigueInfo(SIG_LOAD_ETC* sig)
{
    sig->m_curCharacUsedGiftFatigueQuantity = 0;             // +0xc9b8 清零先

    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    if (evMng == 0 || !evMng->is_eventing(0x65))
    {
        return true;
    }

    bool isCharacEvent = false;
    EventClassify::CUsedFatigueGiveItem* entity =
        static_cast<EventClassify::CUsedFatigueGiveItem*>(
            evMng->get_event_entity(0x65));
    if (entity == 0 ||
        !entity->getIsCharacEvent(isCharacEvent))
    {
        return true;
    }

    MySQL* db = 0;
    const char* tableName =
        toTString(*reinterpret_cast<std::string*>(entity->get_db_table_name()));
    if (isCharacEvent)
    {
        db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                               (ENUM_SERVER_GROUP)0);
        db->set_query("seLect fatigue_quantity from %s where charac_no = %u",
                      tableName, sig->m_characNo);
    }
    else
    {
        db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                               (ENUM_SERVER_GROUP)0);
        char* accStr = NumberToString(sig->m_accId, 0);
        db->set_query("seLect fatigue_quantity from %s where m_id = %s",
                      tableName, accStr);
    }

    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0 || !db->fetch())
    {
        return true;
    }
    unsigned int quantity = 0;
    if (!db->get_uint(0, quantity))
    {
        return false;
    }
    sig->m_curCharacUsedGiftFatigueQuantity = quantity;
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAttendanceInfo @0x08448944 —— SIG_ATTENDANCE_EVENT 是当日
// 是否可签到的 out 参数（+0x00 flag，blob 其余不触）。事件关闭返 0。
// ============================================================================
bool DB_LoadEtc::LoadAttendanceInfo(SIG_ATTENDANCE_EVENT* sig, unsigned int accId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    sig->m_isEnabled = 0;
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x60)->IsEventing((CUser*)0))
    {
        return false;
    }

    long resetBase = GetCurrentResetBaseTime(
        GlobalData::s_systemTime_.getCurSec(),
        G_CEnvironment()->m_serverEnvir.m_fatigueTime /*ORIG +0x37c*/);
    char* accStr = NumberToString(accId, 0);
    db->set_query("seLect occ_date from event_1207_attendance where m_id = %s and"
                  " occ_date > from_unixtime(%u)", accStr, resetBase);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        // ORIG cmpl $0,jne skip → n_rows==0 时 movb $1（当日尚可签）。
        sig->m_isEnabled = 1;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadSeriaRoomAniDeco @0x0844a20a —— seria 房间动画装饰双 short
// 至 +0x14b5c/+0x14b5e（combo 区尾部残段，超 SIG_COMBO_SKILL 0xb2 的部分）。
// ============================================================================
bool DB_LoadEtc::LoadSeriaRoomAniDeco(SIG_LOAD_ETC* sig)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing((CUser*)0))
    {
        return false;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect receive_flow, deco_flow from event_1208_seriaroom_anideco"
                  " where m_id = %d and server_id = %d and charac_no = %d",
                  sig->m_accId, sig->m_serverGroup, sig->m_characNo);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              "bool DB_LoadEtc::LoadSeriaRoomAniDeco(SIG_LOAD_ETC*)",
                              0xba8f, "LoadSeriaRoomAniDeco Query Error : m_id(%d)",
                              sig->m_accId);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_short(0,
                       *reinterpret_cast<short*>(&SGF<unsigned short>(sig, 0x14b5c))))
    {
        return false;
    }
    if (!db->get_short(1,
                       *reinterpret_cast<short*>(&SGF<unsigned short>(sig, 0x14b5e))))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadEventServerMessageInfo @0x084356a2 —— event_server_message
// 按服务器/频道装载进 SIG_LOAD_ETC 条目数组（base +0x2d38 起，stride 0x58）。
// entry 布局（ORIG 写点）：+0x00 kind(byte) | +0x01 index(byte) |
//   +0x08 strlen(size_t) | +0x0c message[50 内] | +0x40 strlen | +0x44 name[20]
//   | +0x58 update_time(int) —— 以文档化偏移直写，条目计数入 +0x2d38。
// ============================================================================
bool DB_LoadEtc::LoadEventServerMessageInfo(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect kind, message_index, charac_name, message,"
                  " unix_timestamp(update_time) from event_server_message where"
                  " server_info=%d and (channel_no=%d or channel_no=0)",
                  sig->m_serverGroup /*+0x2d30*/, sig->m_channelNo /*+0x2d34*/);
    if (!db->exec(true))
    {
        return false;
    }

    int nRows = db->get_n_rows();
    char message[50];
    char name[20];
    char kind = 0;
    char msgIndex = 0;
    int updateTime = 0;
    int written = 0;

    for (int i = 0; i < nRows; ++i)
    {
        if (!db->fetch())
        {
            return false;
        }
        if (!db->get_byte(0, kind))                          // col0 kind
        {
            return false;
        }
        if (!db->get_byte(1, msgIndex))                      // col1 message_index
        {
            return false;
        }
        if (!db->get_str(2, name, 20))                       // col2 charac_name
        {
            return false;
        }
        if (!db->get_str(3, message, 50))                    // col3 message
        {
            return false;
        }
        if (!db->get_int(4, updateTime))                     // col4 update_time
        {
            return false;
        }

        unsigned int base = 0x2d3c + (unsigned int)written * 0x58;
        SGF<unsigned char>(sig, base)     = (unsigned char)kind;
        SGF<unsigned char>(sig, base + 1) = (unsigned char)msgIndex;
        size_t msgLen = strlen(message);
        SGF<unsigned int>(sig, base + 4) = (unsigned int)msgLen;   // +0x2d40 strlen
        strncpy(MIX(sig, base + 8) /*+0x2d44*/, message, msgLen);
        size_t nameLen = strlen(name);
        SGF<unsigned int>(sig, base + 60) = (unsigned int)nameLen; // +0x2d78 strlen
        strncpy(MIX(sig, base + 64) /*+0x2d7c*/, name, nameLen);
        SGF<int>(sig, base + 84) = updateTime;               // +0x2d90 update_time
        ++written;
    }

    SGF<unsigned char>(sig, 0x2d38) = (unsigned char)nRows;  // 条目计数（截断）
    return true;
}

// ============================================================================
// get_skill_table_name @0x083fbdc6：PvP 技能树频道或显式第二表 →
// "skill_fair_pvp.skill"，否则 "skill.skill"。
// ============================================================================
char const* get_skill_table_name(bool use2nd)
{
    if (G_GameWorld()->IsPvPSkilTreeChannel() || use2nd)
        return "skill_fair_pvp.skill";
    return "skill.skill";
}

// ============================================================================
// getComboSkillTableName @0x0844a38b：kind==1 → "combo_skill2"，
// 其余（含 0）→ "combo_skill"。
// ============================================================================
char const* getComboSkillTableName(ENUM_SKILL_TREE_KIND kind)
{
    if (kind == 1)
        return "combo_skill2";
    return "combo_skill";
}
