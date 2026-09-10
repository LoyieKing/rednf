// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_LoadEtc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "DB_LoadEtc/get_compressed_blob_data.inc"
                              int recordSize);
// ORIG T _Z22getDailyInitializeTimeiii @0x086b1f5a（定义 GameUtils.cpp）。
#include "DB_LoadEtc/getDailyInitializeTime.inc"

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ORIG T GetCurrentResetBaseTime(long, int) @0x0810912a
// （定义 LinkClosureA_Misc.cpp）。
#include "DB_LoadEtc/GetCurrentResetBaseTime.inc"

// ============================================================================
// DB_LoadEtc::DB_LoadEtc @0x08403b84（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x08403ba0 设 vptr→基类空 dtor→恒假 delete 分支；
// D0 0x08403bd0 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
#include "DB_LoadEtc/DB_LoadEtc.inc"
{
}

// ============================================================================
// ~DB_LoadEtc @0x08403ba0（D1/D2）/ deleting dtor @0x08403bd0（D0）
// ============================================================================
#include "DB_LoadEtc/dtor_DB_LoadEtc.inc"
{
}

// ============================================================================
// DB_LoadEtc::GetUserRecord @0x084075c4：纯 ret 1（单指令函数体）。
// ============================================================================
#include "DB_LoadEtc/GetUserRecord.inc"
{
    (void)sig;
    // ORIG 0x084075c4 六指令含 `sub $0x10,%esp`+leave：4.1 对本函数体预留帧空间
    // （无 call 也展开帧），dummy 数组令 4.4 同形（P31；16B 边界内不触发 4.4
    // 的 32B 对齐，实测同形）。
    char frame[0x10];
    (void)frame;
    return 1;
}

// ============================================================================
// DB_LoadEtc::GetUserPvpBase @0x08407db4：纯 ret 1。
// ============================================================================
#include "DB_LoadEtc/GetUserPvpBase.inc"
{
    (void)sig;
    char frame[0x10];          // ORIG 0x08407db4 sub $0x10 + leave 同形（P31）
    (void)frame;
    return 1;
}

// ============================================================================
// DB_LoadEtc::GetUserPvp @0x08407dc2。
// 26 列 pvp_result 读取入 sig+0x340 PvpResultType；随后按本地当日零点重算
// m_nextResetTime(+0x68)，若晚于 last_play_time(col22) 清 +0x60；
// 若 pvp_grade_ext_update_time(col25)+86400 < 当前秒清 +0x5c。返 0=OK
// 1=无行 -1=DB/取值错误。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x08407dc2；385 vs 390（帧/槽），p167 待核。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=739 (orig=385/ours=390)：N82 帧/槽错位（4.1 不复用
// 栈槽 vs 4.4 复用）+ N95 槽基差；p167 CLEAR 0/0、gap=5 ⇒ 语义完成、byte 层封顶（§6.2）。
// [b28d-SweepDBLoadEtc 本波] 具名 nowSec 槽（ORIG `call getCurSec; mov %eax,-0xc` +
// `cmp -0xc(%ebp),%eax; jge` 直跳）：旧内联 getCurSec() 致 4.4 setcc 物化；改后 385/386
//（gap 5→1）、p167 CLEAR。
// [b28d-SweepDBLoadEtc2 本波] **DIFF→AE（385/385）**，三处源形按报告逐条复现：
//   ①具名 `PvpResultType* pvp = &sig->m_pvpResult;`（ORIG 0x08407e45 `mov 0xc(%ebp),%eax;
//     add $0x340,%eax; mov %eax,-0x14`，P0 槽-局部对应律；旧直写 sig->m_pvpResult 使
//     sig 形参使用 28 次 vs ORIG 2 次）；
//   ②get_uint(23/24) 直接传字段地址 `(unsigned int&)pvp->m_todayPlayCount/m_todayLoseCount`
//     （ORIG 0x08408273/0x084082a1 `pvp+0x64/0x60`，无中间 u 局部，P0）；
//   ③localtime_r 改 P43 指针形 `struct tm* t = localtime_r(&now,&tmv);`，写序照 ORIG
//     +0x8→+0x4→+0x0（tm_hour/tm_min/tm_sec）；mktime 结果直存 +0x68 后重读比较（无
//     nextReset 局部）；gradeExtUpdateSec 裸声明（ORIG 0x08408338 无前置清零）。
#include "DB_LoadEtc/GetUserPvp.inc"
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

    // sig+0x340 = m_pvpResult（PvpResultType，权威字段布局见 PvpResultType.h；
    // +0x0c/+0x10 两列 DB 不读）。ORIG 0x08407e45 `mov 0xc(%ebp),%eax;
    // add $0x340,%eax; mov %eax,-0x14(%ebp)` ⇒ 具名 PvpResultType* 局部
    // （P0 槽-局部对应律：有槽往返 ⇒ 原码有该局部），后续逐列 get_int（unrolled）：
    // win/lose/pvp_point→m_totalWinCount/m_totalLoseCount/m_expPoint，
    // pvp_grade→m_pvpGrade、avg[15]→m_avg[0..14]、pvp_count→m_count、
    // win_point→m_winPoint、pvp_grade_ext→m_todayWinCount。
    PvpResultType* pvp = &sig->m_pvpResult;
    if (!db->get_int(0, pvp->m_totalWinCount)) return -1;   // win
    if (!db->get_int(1, pvp->m_totalLoseCount)) return -1;  // lose
    if (!db->get_int(2, pvp->m_expPoint)) return -1;        // pvp_point
    if (!db->get_int(3, pvp->m_pvpGrade)) return -1;        // pvp_grade
    if (!db->get_int(4, pvp->m_avg[0])) return -1;
    if (!db->get_int(5, pvp->m_avg[1])) return -1;
    if (!db->get_int(6, pvp->m_avg[2])) return -1;
    if (!db->get_int(7, pvp->m_avg[3])) return -1;
    if (!db->get_int(8, pvp->m_avg[4])) return -1;
    if (!db->get_int(9, pvp->m_avg[5])) return -1;
    if (!db->get_int(10, pvp->m_avg[6])) return -1;
    if (!db->get_int(11, pvp->m_avg[7])) return -1;
    if (!db->get_int(12, pvp->m_avg[8])) return -1;
    if (!db->get_int(13, pvp->m_avg[9])) return -1;
    if (!db->get_int(14, pvp->m_avg[10])) return -1;
    if (!db->get_int(15, pvp->m_avg[11])) return -1;
    if (!db->get_int(16, pvp->m_avg[12])) return -1;
    if (!db->get_int(17, pvp->m_avg[13])) return -1;
    if (!db->get_int(18, pvp->m_avg[14])) return -1;
    if (!db->get_int(19, pvp->m_count)) return -1;          // pvp_count
    if (!db->get_int(20, pvp->m_winPoint)) return -1;       // win_point
    if (!db->get_int(21, pvp->m_todayWinCount)) return -1;  // pvp_grade_ext

    int lastPlaySec = 0;
    if (!db->get_int(22, lastPlaySec))              // unix(last_play_time)→局部
        return -1;
    // ORIG 0x08408273/0x084082a1：get_uint 直接传 pvp+0x64 / pvp+0x60（无中间局部）。
    if (!db->get_uint(23, (unsigned int&)pvp->m_todayPlayCount))
        return -1;
    if (!db->get_uint(24, (unsigned int&)pvp->m_todayLoseCount))
        return -1;

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    // ORIG 0x084082f0：localtime_r 返回值落具名 tm* 局部，成员经指针写（P43）。
    struct tm* t = localtime_r(&now, &tmv);
    // ORIG 0x084082f6..0x0840830a 写序 +0x8 → +0x4 → +0x0（tm_hour/tm_min/tm_sec）。
    t->tm_hour = 0;
    t->tm_min = 0;
    t->tm_sec = 0;
    // ORIG 0x0840831b：mktime 结果直存 +0x68（无 nextReset 局部），随后重读字段比较。
    pvp->m_nextResetTime = (int)mktime(t);
    if (pvp->m_nextResetTime > lastPlaySec)
        pvp->m_todayLoseCount = 0;                  // play_time 清零

    // ORIG 0x08408338 直接 lea -0x24（无前置清零）⇒ 裸声明。
    int gradeExtUpdateSec;
    if (!db->get_int(25, gradeExtUpdateSec))        // unix(pvp_grade_ext_update_time)
        return -1;
    // ORIG 0x08408360：getCurSec 返回值先落具名槽（mov %eax,-0xc）再与
    // gradeExtUpdateSec+0x15180 直跳比较（无 setcc 物化）。
    int nowSec = (int)GlobalData::s_systemTime_.getCurSec();
    if (gradeExtUpdateSec + 0x15180 < nowSec)
        pvp->m_todayWinCount = 0;                   // pvp_grade_ext 清零

    return 0;
}
// ============================================================================
// DB_LoadEtc::LoadLitmitItem @0x08405c60：sub $0x30 后直接 ret 1（如实空体）。
// ============================================================================
#include "DB_LoadEtc/LoadLitmitItem.inc"
{
    (void)sig;
    char frame[0x30];          // ORIG 0x0840xxxx sub $0x30,%esp + leave 同形
    (void)frame;
    return true;
}

// ============================================================================
// DB_LoadEtc::dispatch @0x08403bee
// 调用序/分支/错误行号严格按报告汇编（0x14c6/0x14db/0x14e3/0x1566 等）。
// ============================================================================
// [b29-Lane21] **AE（残差 0）**：`bool sendOk` 提前声明（先声明→更深槽）+ 两句式赋值
//   ⇒ ORIG 槽集 `-0xc(41)/-0xd(2)/-0x20(2)/-0x24(4)` 逐值全等（判据⑩）。
// b28d-31: N 封顶（N82）——ORIG 0x08403bee；我们多 3 条（帧/装载序），
// p167 待核，语义按 dispatch 协议等价。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=353 (orig=323/ours=326)：N82 帧/槽 + 本 TU callee
// 定义可见导致的帧继承差；p167 CLEAR 0/0 ⇒ 语义完成、byte 层封顶（§6.2）。
// [b28d-SweepDBLoadEtc2 本波] **DIFF→NEAR（327 vs 323，助记符序列全等，残差仅槽号
//   sig -0x10 vs ORIG -0xc / 装载序）**，并修真 bug：
//   ①`GetUserPvp` 判定改 **switch**（ORIG 0x08403c69 无槽 `test/cmp $1/cmp $-1/jmp` 链，
//     P251/P261；内层 GetUserPvpBase 同形），case 叶块物理序按 ORIG（case1→case-1→case0，
//     P324）；
//   ②LoadLitmitItem 错误路径 cMyTrace 改 P227 全表达式临时链（ORIG 0x08403d80 ctor 前
//     `mov 0x4(%eax),%ebx`）；
//   ③SendResult 改具名 bool 局部（ORIG 0x08404073 `mov %al,-0xd; movzbl; xor $1`，P102）；
//   ④**真 bug**：删 `if (!LoadQuestShop(sig)) return false;` 多余门禁——ORIG 0x08403f5e
//     调用后直接 LoadTitleBook（返回值丢弃），旧形在 LoadQuestShop 失败时提前返回 false。
#include "DB_LoadEtc/dispatch.inc"
{
    // [b29-Lane21] P212 槽序反转律：ORIG sendOk@sig 更深（-0xd vs sig -0xc）
    // ⇒ sendOk 原码声明在 sig 之前（先声明→更深槽）。
    bool sendOk;
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

    // ORIG 0x08403c69..0x08403c77 无栈槽多路比较链
    //   `test %eax,%eax; je; cmp $0x1; je; cmp $0xffffffff; je; jmp`
    // ⇒ 原码是 switch(GetUserPvp(sig))（P251/P261），if-else 链恒带状态槽。
    // case 叶块物理序 = 书写序（P324）：ORIG 块序为 case1(+0x8e) → case-1(+0xd8)
    // → case0(+0xea)，决策链由编译器生成（P46）。
    switch (GetUserPvp(sig))
    {
    case 1:
    {
        DB_CreateCharac createCharac;                 // C1 0x08400cc2
        createCharac.CreatePvP(sig->m_characNo);      // T 0x084026ec
        break;                                        // D1 0x08400cde + try 帧
    }
    case -1:
        sig->m_pvpResult.Clear();
        break;
    case 0:
    {
        // 内层同形：ORIG 0x08403cea `cmp $0xffffffff; je; cmp $0x1; je; jmp`（P251），
        // 块序 -1(+0x108) → 1(+0x11a)。
        switch (GetUserPvpBase(sig))
        {
        case -1:
            sig->m_pvpResult.Clear();
            break;
        case 1:
            sig->m_pvpResult.m_field10 = -1;    // ORIG sig+0x350（PvpResultType+0x10）
            break;
        }
        break;
    }
    }

    if (!LoadCharacItemStat(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x14c6, "DB_LoadETC ERROR : LoadCharacItemStat Fail");
        return false;
    }

    if (!LoadLitmitItem(sig))
    {
        // ORIG 0x08403d7d..0x08403db8：ctor 前 `mov 0x4(%eax),%ebx` 缓存
        // sig->m_characNo，ctor 后才 mov %ebx,0x8(%esp) ⇒ 全表达式临时链（P227）。
        cMyTrace(__PRETTY_FUNCTION__, 0x14db, 0)(
            "LoadLimitItem middle fetch <charac:%d>", sig->m_characNo);
        return false;
    }

    if (!GetUserSkill(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
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
                              __PRETTY_FUNCTION__,
                              0x1566, "DB_LoadETC ERROR : LoadItemMakingSkill Fail");
        return false;
    }
    // ORIG 0x08403f5e..0x08403f6a：LoadQuestShop 返回值**丢弃**（call 后直接 LoadTitleBook，
    // 无 test/je）⇒ 原码为语句调用；旧 `if (!LoadQuestShop(sig)) return false;` 是多余门禁
    // （真 bug：失败时 ORIG 继续后续 Load，我方提前返回 false）。
    LoadQuestShop(sig);

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

    // ORIG 0x0840404c 段：`mov %al,-0xd(%ebp); movzbl -0xd(%ebp),%eax; xor $1`
    // ⇒ 具名 bool 局部物化返回值（P102/P158），随后失败路径 `mov $0,%eax`。
    sendOk = SendResult(userIdx, slot, sig);
    if (!sendOk)
    {
        return false;
    }

    WongWork::EventCommon::LoadCharacEventItem(
        sig->m_accId, sig->m_characNo, 0);
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadProperDungeonClearCount @0x084073aa
// ============================================================================
#include "DB_LoadEtc/LoadProperDungeonClearCount.inc"
{
    // ORIG 槽序：db=-0x10（先声明→深）、nRows=-0xc（后声明→浅）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect clear_cnt from event_dungeon_clear where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();
    if (nRows != 1)
    {
        sig->m_properDungeonClearCount = 0;          // +0x40cc word 清零
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_short(0,
                       *reinterpret_cast<short*>(&sig->m_properDungeonClearCount)))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadConditionEventInfo @0x8407296？—— 报告头地址 0x08407296；
// 列 current_step(+0x40c8)/reward_step(+0x40ca)，缺行清零。
// ============================================================================
#include "DB_LoadEtc/LoadConditionEventInfo.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect current_step, reward_step from event_conditionable_info"
                  " where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();   // 后声明→浅槽 -0xc（ORIG 形）
    if (nRows != 1)
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
                       *reinterpret_cast<short*>(&sig->m_curConditionEventStep)))
    {
        return false;
    }
    if (!db->get_short(1,
                       *reinterpret_cast<short*>(&sig->m_curConditionEventRewardStep)))
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
// b28d-31: N 封顶（N82/N95）——ORIG 0x08407482；槽号 -0x10/-0xc 整体平移
// （4.4 局部槽基策略），助记符序列等价。
// [b28d-solo-DB_LoadEtc] AE ndiff=0 (orig=86/ours=86)：具名 nRows 槽（P0 槽律）+ get_uint
// 直写 sig->m_npcCount（去 npcCnt 中间局部）+ 删 static_cast<int> 中转（P265 档②）。
#include "DB_LoadEtc/_loadNPCRelasionship.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect npc_cnt,npc_data from charac_npc where charac_no=%d",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return 0x1c80;
    }
    int nRows = db->get_n_rows();                            // ORIG -0xc 具名槽 + cmpl mem
    if (nRows != 1)
    {
        return 0x1c84;
    }
    if (!db->fetch())
    {
        return 0x1c87;
    }
    // ORIG 0x08407526：get_uint 目标直接是 sig+0x3e6c（无中间局部，P0 槽律）。
    if (!db->get_uint(0, sig->m_npcCount))
    {
        return 0x1c8a;
    }
    if (db->get_binary_length(1) != sig->m_npcCount * 12)
    {
        return 0x1c8d;
    }
    if (!db->get_binary(1, sig->m_npcData, 600))  // +0x3e70，cap 0x258
    {
        return 0x1c90;
    }
    return 0;
}

// ============================================================================
// DB_LoadEtc::LoadRedeemItem @0x846a6a？—— 报告头地址 0x08406a6a；
// redeem_info blob → uncompress_zip → SIG_LOAD_ETC+0x40ce（m_expandData6 区）。
// ============================================================================

// [b29-Lane21] 已修：`unsigned int srcLen` + 去 cast ⇒ dst 先算，ORIG 装载序复现。
// 残差（N 封顶）：N7 寄存器角色（ORIG `mov sig,%eax; add $0x40ce,%eax` 就地加；4.4
//   `mov sig,%eax; lea 0x40ce(%eax),%edx`）——成员名/`&arr[0]`/cast/匿名 struct 视图
//   四形实测均不翻，语义等价（161=161）。
// [b28d-solo-DB_LoadEtc] N封顶（N7）：ORIG 0x08406a6a；161=161、p167 CLEAR 14/14。
// 已修（本波）：①`char blob[0x6c7] = { 0 };` 替 memset call（P214③/P253；ORIG 内联
//   rep stos 慢路 + 尾字节，指令数 129→161 与 ORIG 全等）；②get_binary 第 3 参复用
//   outLen 具名槽（P244；原直填 0x6c7 使 4.4 折常量）。
// 残差（不可翻，N7）：uncompress_zip 实参装载的整组寄存器角色互换——ORIG
//   `mov 0xc(%ebp),%eax; add $0x40ce,%eax` + `mov -0xc(%ebp),%edx`（dst→eax）；
//   4.4 恒 `mov -0xc(%ebp),%eax` + `mov 0xc(%ebp),%edx; add $0x40ce,%edx`（dst→edx）。
// 助记符序列与槽位全等，仅 eax/edx 角色互换（P230 补 N7 族；446 后端已应用）。
// 已修（前波）：exec/fetch 具名 ok + 三处 trace 改 P227 临时链。
#include "DB_LoadEtc/LoadRedeemItem.inc"
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc14LoadRedeemItem…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect redeem_info from charac_inven_expand where charac_no=%u",
                  sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b16, 5)(
            "DB_LoadRedeemItemList::dispatch, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b20, 5)(
            "DB_LoadRedeemItemList::dispatch, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    // [b28d-solo-DB_LoadEtc] `={0}` 触发 4.4 内联 rep stos（P214③；原 memset 生 call），
    //   与 LoadPvPMissionList 同法；get_binary 第 3 参复用 outLen 具名槽（P244）。
    char blob[0x6c7] = { 0 };
    unsigned int outLen = 0x6c7;
    if (!db->get_binary(0, blob, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b28, 5)(
            "DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    // [K2ADBLogin 2026-09-11] 负结论（本波实测）：ORIG 0x08406c88 的 dst 形
    //   `mov 0xc(%ebp),%eax; add $0x40ce,%eax`（原地 add、结果留 %eax）源侧两种写法
    //   均不可复现——成员数组退化与显式指针算术 `(char*)sig + 0x40ce` 都产
    //   `lea 0x40ce(%eax),%edx`（4.4 地址生成恒 lea + 结果入 %edx）⇒ N107 反向
    //   （ORIG add / 4.4 lea）+ N7 寄存器角色，判封顶（读数 161/161 不变）。
    if (!uncompress_zip(sig->m_expandData6, outLen, blob, srcLen))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadPvPMissionList @0x08406cce —— fair_pvp_score.pvp_mission_info
// blob → SIG_LOAD_ETC+0xcdd5（type8 expand 区，cap 0x198）。n_rows==0 也返 0。
// ============================================================================
// [b29-Lane21] **已归零（残差 0）**：`unsigned int srcLen` + 去 cast ⇒ ORIG
//   `mov 0xc(%ebp),%eax; add $0xcdd5,%eax; mov -0xc(%ebp),%edx`（dst 先算）复现。
// [b28d-solo-DB_LoadEtc] N封顶（P75 实参装载序）：ORIG 0x08406cce。
// 已修（本波）：①blob 改 `char blob[0x198] = { 0 };`（内联 rep stos，替 memset call，
//   P214③；帧 push edi/ebx + sub 0x1f0 对齐）；②`get_binary(0, blob, outLen)` 第 3 参
//   复用 outLen 具名槽（原直填 0x198 使 4.4 折常量，P244）。
// 残差（不可翻，P75）：uncompress_zip 实参装载序——ORIG 先 dst（`mov sig,%eax;
//   add $0xcdd5,%eax`）后 srcLen；4.4 恒先 srcLen。余者全等（含槽位与 rep stos）。
//   [b28d-solo 复测] 133=133、唯一 hunk 即该 3 行（srcLen 槽读与 dst 计算的先后）；
//   探针 p4.cpp e4（srcLen 改 unsigned）仅把 dst 换成 lea+edx 形态，仍差 ⇒ N7 封顶。
// 已修（前波）：exec/fetch 具名 ok + trace 临时链。
#include "DB_LoadEtc/LoadPvPMissionList.inc"
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc18LoadPvPMissionList…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect pvp_mission_info from fair_pvp_score where charac_no=%u",
                  sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b3f, 5)(
            "LoadPvPMissionList, exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b49, 5)(
            "LoadPvPMissionList, fetch() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }

    // ORIG 0x08406d…：`char blob[0x198]={0}` 内联 rep stos（P214③，非 memset call）；
    // outLen 具名槽写后读回（P244：get_binary 第 3 参复用 outLen 槽，非直填 0x198）。
    char blob[0x198] = { 0 };
    unsigned int outLen;
    outLen = 0x198;
    if (!db->get_binary(0, blob, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b51, 5)(
            "DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    if (!uncompress_zip(sig->m_pvpMissionInfo, outLen, blob, srcLen))
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
// [b29-Lane21] 已修：`unsigned int srcLen` ⇒ 实参装载序 srcLen 位置差消。
// 残差（N 封顶）：①N1 常量拆分（ORIG `mov sig,%ecx; …; add $0xb1a0,%eax; mov 0x4(%eax),%eax`
//   vs 4.4 `add $0xb1a4,%eax; mov (%eax),%eax`——元素内 +4 被折入基址常量）；
//   ②N7 dst 取址 add/lea + `mov 0xc(%ebp),%ecx` 装载序。
// [b28d-solo-DB_LoadEtc] N封顶（N1 常量合并 + P75 实参装载序）：ORIG 0x08406422。
// 已修（本波）：count 循环改 12 字节元素视图 `AchievementEntry m_entries[0x200]`
//   （基址 0xb1a0 + 元素内 +4，替代 `m_expandDataF + i*12` 的 cast 形）。
// 残差（不可翻，两处，余者全等 + nop 豁免）：
//   ①uncompress_zip 实参装载序——ORIG 先第 1 参（`mov sig,%eax; add $0xb1a4,%eax`）后
//     第 4 参 srcLen；4.4 恒先第 4 参（P75 跨版本实参装载时刻；删冗余 cast 无效）。
//   ②循环基址——ORIG `lea (sig,i*12); add $0xb1a0,%eax; mov 0x4(%eax)`（基址 sig 原始、
//     disp4 保留）；4.4 恒把基址偏移与元素内偏移合并为 `add $0xb1a4` + `mov (%eax)`
//     （N1 常量折叠；offset-0/成员数组/short 元素三形实测均合并）。
//     [b28d-solo 复测] 补测第四形：二维具名数组视图 `int m_entries[0x200][3]` +
//     `m_entries[i][1]` ⇒ 4.4 仍折 `add $0xb1a4` + `mov (%eax)`（P311 于本偏移不适用），
//     127=127 条；四形均合并 ⇒ 封顶。
// 已修（前波）：exec 具名 ok、outLen 顺序、get_uint 直写 m_achievementLastUpdate（去局部）。
#include "DB_LoadEtc/LoadAchievement.inc"
{
    static const int compressed_buff_size = 0x1cd8;  // ORIG _ZZ…LoadAchievement…E20compressed_buff_size
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect achievement, unix_timestamp(last_update_time) from"
                  " charac_achievement where charac_no=%u", sig->m_characNo);
    bool ok = db->exec(true);                                // ORIG -0x15 具名槽
    if (!ok)
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

    // ORIG 0x084064cf：rep stos edx=0x736 **dword** 计数 = 0x1cd8 字节（P253 全清），
    // `={0}` 触发 4.1/4.4 的 rep stos 展开（P214③）。
    char blob[0x1cd8] = { 0 };
    if (!db->get_binary(0, blob, compressed_buff_size))
    {
        return false;
    }
    unsigned int srcLen = db->get_binary_length(0);
    unsigned int outLen = 0x1800;      // ORIG 0x08406531：srcLen 之后才初始化
    // [K2ADBLogin 2026-09-11] 负结论（同 LoadRedeemItem）：显式指针算术形实测
    //   仍产 `lea 0xb1a4(%eax),%edx`（ORIG 为 `add $0xb1a4,%eax`）⇒ N107 反向封顶。
    if (!uncompress_zip(sig->m_expandDataF, outLen, blob, srcLen))
    {
        return false;
    }

    int count = 0;
    // ORIG 0x0840658c：`mov sig,%ecx; i*12; lea (%ecx,%eax,1),%eax; add $0xb1a0,%eax;
    //   mov 0x4(%eax),%eax`——基址偏移 0xb1a0 非 12 倍数 ⇒ 4.4 保留独立 add（不折 disp）。
    struct AchievementEntry
    {
        int m_field0;
        int m_flag;      // +4（ORIG 读点）
        int m_field8;
    };
    struct AchievementView
    {
        char m_pad[0xb1a0];
        AchievementEntry m_entries[0x200];
    };
    for (int i = 0; i <= 0x1ff; ++i)                       // i <= 0x1ff（cmpl 0x1ff setle）
    {
        if (((AchievementView*)sig)->m_entries[i].m_flag == 0)
        {
            break;                                          // 计数停在首个零条目
        }
        ++count;
    }
    sig->m_achievementCount = count;             // +0xc9a4（movl）

    // ORIG 0x084065c4：get_uint 目标直接是 sig+0xc9a8（无 lastUpdate 局部，P0 槽律）。
    if (!db->get_uint(1, sig->m_achievementLastUpdate))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadOnceGiveAvengerTitle @0x08406600
// ============================================================================
#include "DB_LoadEtc/LoadOnceGiveAvengerTitle.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    // 结果不入槽：ORIG 8904 内联实参直传（无 accStr 保存对）。
    db->set_query("seLect m_id from event_1107_avenger_plan where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_flagPresentAvengerTitle = 1;                 // +0xc9ae movb $1 直写
        return false;
    }
    int nRows = db->get_n_rows();       // 后声明→浅 -0xc（ORIG cmpl $0,-0xc 保存比较形）
    if (nRows != 0)
        sig->m_flagPresentAvengerTitle = 1;           // movb $1,0xc9ae(%eax)
    else
        sig->m_flagPresentAvengerTitle = 0;           // movb $0,0xc9ae(%eax)
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAuraAvatarOption @0x0840838e
// aura_avatar_option → stAuraAvatarOption 视图 @SIG_LOAD_ETC+0xd730。
// ============================================================================
// [b28d-solo-DB_LoadEtc] **AE（残差 0；92=92、p167 CLEAR 8/8）**：ORIG 0x0840838e。
// 已修（本波，本件归零）：①两步寻址 `add $0x35cc,%ecx` + `mov %edx,(%eax,%ecx,4)`——原
//   数组视图位于 offset 0 使 4.4 折 `lea 0x35cc(%edx),%ecx`；改成员数组位于真实偏移
//   0x35cc*4 的视图（P30）。
//   ②store 装载序——ORIG 0x084084af 为 index→value→base；跨类型 cast 视图恒 base-first
//   （探针 /tmp/dble_probe/p8.cpp k4），改 **SIG_LOAD_ETC 完整类型成员直访问**
//   `sig->m_auraOptionWords[optionType]`（SigTypes_C.h +0xd730 匿名 union 视角，Main 代落，
//   P235）复现 index→value→base（同探针 k1）；元素 unsigned int 与 RHS 异型消 4.4 的
//   int→unsigned 装载中转（探针 p9.cpp m2）。旧「N76 不可翻」判词作废。
// 残差：无（92=92、p167 CLEAR 8/8）。
#include "DB_LoadEtc/LoadAuraAvatarOption.inc"
{
    // ORIG 0x08408394：无 aura 具名局部（add $0xd730 直传 this，P0 槽律）。
    reinterpret_cast<stAuraAvatarOption*>(sig->m_auraAvatarOption)->reset();

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
    // ORIG 0x08408423：fetchOk 预清两句式（P198①）+ while 前置 jmp cond（P25）；
    // fetch 失败 break 出循环（非 return）。
    bool fetchOk = false;
    int i = 0;
    while (i < nRows)
    {
        fetchOk = db->fetch();
        if (!fetchOk)
        {
            break;
        }
        int optionType;                 // ORIG 无预清（get_int 直写）
        unsigned int value;             // [b28d-solo] 与 union 视角元素同型，消 4.4 的
                                        // int→unsigned 装载中转（探针 /tmp/dble_probe/p9.cpp m2）
        if (!db->get_int(0, optionType))
        {
            return false;
        }
        if (!db->get_int(1, *reinterpret_cast<int*>(&value)))
        {
            return false;
        }
        if (-1 < optionType && optionType < 3)
        {
            // ORIG 0x084084af：mov -0x1c,%ecx; mov -0x20,%edx; mov 0xc(%ebp),%eax;
            //   add $0x35cc,%ecx; mov %edx,(%eax,%ecx,4)
            // ⇒ sig 的 uint 数组成员（下标含常量 0x35cc），P30 两步寻址。
            // [b28d-solo-DB_LoadEtc] P235：必须是 SIG_LOAD_ETC 的**完整类型成员**直访问
            //   （SigTypes_C.h +0xd730 匿名 union 视角 m_auraOptionWords，Main 代落）；
            //   跨类型 cast 视图恒产 base-first（探针 /tmp/dble_probe/p8.cpp k4），
            //   成员直访问逐条复现 ORIG 的 index→value→base（同探针 k1）。
            sig->m_auraOptionWords[optionType] = value;
        }
        ++i;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadCharacOption @0x084084e0
// options blob(+0xeac8,0x200)/best_clear_time blob(+0xec d8,24000)/负值钳位
// (+0xECDC..)/blueMarbleEnterCount(+0x14a98)/charac_inform_notice(+0x14a9a,16B)
// 及 best_clear_time 条目计数 +0xecd4。
// ============================================================================
// [b28d-solo-DB_LoadEtc] N封顶（N82/N107/N30）：ORIG 0x084084e0；168 vs 171、p167 CLEAR 11/11。
// 已修（本波）：列号改**自增具名局部** `int col = 0; … get_binary(col++, …)` ⇒ 复现 ORIG
//   的 `mov -0x18(%ebp),%eax; addl $0x1,-0x18(%ebp)`（原字面量列号使 4.4 直填常量
//   `movl $0x1,0x4(%esp)`，-10 条）。
// 残差（不可翻）：①N107/N30 地址形（`lea 0xeac8(%eax),%edx` vs `add $0xeac8,%eax`；
//   `mov 0xc(%eax,%edx,4)` 两步 vs 4.4 add+shl 链）；②N82 槽基差 4（db 槽 -0x20 vs
//   ORIG -0x1c；col 槽引入后整体深 4）；③trace 槽深差。
// [b28d-SweepDBLoadEtc 本波] 真 bug 修复：条目计数循环原 `for+if` 遍历全表统计非零数，
// ORIG 0x08408734 `test %eax,%eax; je 0x0840874e` 首条 0 即 break（Ghidra C
// `while (i<1000 && *(int*)(sig+i*0x18+0xecd8)!=0){++count;++i;}`）⇒ 改 while+break，
// 计数=从头连续非零条数。读数 168/171→168/172（残差 N1 折叠）。
// [b28d-SweepDBLoadEtc2 本波] 真 bug（读错偏移）：计数循环原读
//   `m_bestClearTime + idx*0x18 + 8`（=sig+0xece0+24*idx，元素内第 3 个 int），ORIG
//   0x08408718..0x0840873c 为 `lea (%ecx,%eax,1); add $0xecd0; mov 0x8(%eax)` = 
//   sig+0xecd8+24*idx（**元素首 int**）⇒ 删 +8。另 trace 改 P227 临时链
//   （ORIG 0x0840859f ctor 前 `mov 0x4(%eax),%ebx` 缓存 characNo）。
//   残差：N107/N30 地址形 + N82 槽基差 4（不可翻）。
// [b29-Lane21 复核] 四旋钮定位（ndiff 82→16，orig 168/our 170）：
//   ①删 `int* p` 指针局部改表达式直用（P0：ORIG 无指针槽往返）⇒ 整体槽基回退 4，
//     槽集与 ORIG 全等（-0xc/-0x10/-0x14/-0x18/-0x1c/-0x2c）；
//   ②全头部声明 count/i/idx（P212 按 ORIG 槽深序 db→col→count→i→idx）⇒ i/idx 槽位归位；
//   ③负值钳位改 sig 基址视图 `m_clamp[i+0x3b34]`（P291）⇒ 逐字复现 `mov 0xc(%eax,%edx,4)`；
//   ④条目计数改 `m_rows[idx].m_first`（行基址 +0xecd0/步长 0x18/行内 +8）。
// 残差（不可翻）：①N107 取址形 ×2（`lea 0xeac8(%eax)`/`lea 0x14a9a(%eax)` vs 4.4
//   `add+mov`）；②N4/P30 装载序（ORIG 先装索引 %edx 后装 %eax 基址，4.4 相反）。
// LoadCharacOption 负值钳位窗口视图：m_clamp 数组起 +0xC（元素 4 字节、索引
// i+0x3b34 落 sig+0xECDC）；具名数组成员让 4.4 产 ORIG 的
// `mov 0xc(%eax,%edx,4)`（edx=i+0x3b34）scaled 索引形（P291/P30）。
struct SigLoadEtcNegClampView
{
    char m_pad[0xc];               // +0x000..+0x00B（= m_fieldC 前的字段区）
    int  m_clamp[0x3b34 + 5];      // +0x00C..（索引 i+0x3b34 即 sig+0xECDC+i*4）
};

// LoadCharacOption options blob 视图（+0xEAC8）：具名成员数组经视图访问 ⇒
// 4.4 复现 ORIG `mov 0x14(%ebp),%eax; lea 0xeac8(%eax),%edx`（数组名退化直传恒折
// `add $0xeac8,%eax; mov %eax,%edx`）。
struct SigLoadEtcOptionsView
{
    char m_pad[0xeac8];
    char m_options[0x200];         // +0xEAC8
};


// LoadCharacOption charac_inform_notice 视图（+0x14A9A = attendanceEvent+1）：
// 视图基址=sig ⇒ ORIG `mov 0x14(%ebp),%eax; lea 0x14a9a(%eax),%edx`（cast/加法取址
// 恒折 `add $0x14a99,%eax; lea 0x1(%eax),%edx`）。
struct SigLoadEtcNoticeView
{
    char m_pad[0x14a9a];
    char m_notice[0x10];           // +0x14A9A（charac_inform_notice）
};

// LoadCharacOption best_clear_time 条目视图：行基址 sig+0xECD0、步长 0x18、
// 首 int 在行内 +8（ORIG `lea (%ecx,%eax,1); add $0xecd0; mov 0x8(%eax)`）。
struct SigLoadEtcBestClearTimeView
{
    struct Row
    {
        char m_pad8[8];
        int  m_first;                  // 行内 +0x08
        char m_rest[0xc];
    };
    char m_pad[0xecd0];                // +0x0000..+0xECCF
    Row  m_rows[1000];                 // +0xECD0，步长 0x18
};

#include "DB_LoadEtc/LoadCharacOption.inc"
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

    // [b28d-solo-DB_LoadEtc] ORIG 用自增列号局部（-0x18，`mov -0x18,%eax; addl $1,-0x18`
    //   取后自增）；原字面量列号使 4.4 直填常量（`movl $0x1,0x4(%esp)`）。
    int col = 0;
    // P212 全头部声明 + ORIG 槽深序（db-0x1c→col-0x18→count-0x14→i-0x10→idx-0xc）。
    int count;
    int i;
    int idx;
    if (!db->get_binary(col++, reinterpret_cast<SigLoadEtcOptionsView*>(sig)->m_options,
                        0x200))                                 // col0 options
    {
        return false;
    }
    if (!get_compressed_blob_data(db, col++, sig->m_bestClearTime, 24000, 0))
    {                                                        // col1 best_clear_time
        // ORIG 0x0840859f：ctor 前 `mov 0x4(%eax),%ebx` 缓存 characNo（P227 临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1f5d, 5)(
            "DB_LoadEtc::LoadCharacOption best_clear_time, get(%d) ERROR"
            " charac_no=%u", col /*local_18 已自增*/, sig->m_characNo);
        return false;
    }

    for (i = 0; i <= 4; ++i)                                 // base sig+0xC+(i+0x3b34)*4
    {
        // ORIG `mov 0xc(%eax,%edx,4)`（edx=i+0x3b34）= sig+0xECDC+i*4：具名数组成员
        // 视图（P291）产 scaled 索引；P0：无指针局部槽往返。
        if (reinterpret_cast<SigLoadEtcNegClampView*>(sig)->m_clamp[i + 0x3b34] < 0)
        {
            reinterpret_cast<SigLoadEtcNegClampView*>(sig)->m_clamp[i + 0x3b34] = 0;
        }
    }

    sig->m_blueMarbleEnterCount = 0;                          // 先置 0 再回填
    if (!db->get_ubyte(col++, sig->m_blueMarbleEnterCount))
    {
        return false;
    }
    // charac_inform_notice @+0x14a9a = attendanceEvent(+0x14a99)+1 重叠区。
    if (!db->get_str(col++, reinterpret_cast<SigLoadEtcNoticeView*>(sig)->m_notice, 16))
    {
        return false;
    }

    count = 0;
    idx = 0;
    // ORIG 0x08408718..0x0840873c：地址 = sig + idx*0x18 + 0xecd0 + 8 = +0xecd8 + idx*24
    // （`lea (%ecx,%eax,1); add $0xecd0; mov 0x8(%eax)`，P185 直读）——读**元素首 int**；
    // 旧式 `m_bestClearTime + idx*0x18 + 8` 多 8 字节（真 bug：读元素内第 3 个 int）。
    // 语义：首条 0 即 break（`test %eax,%eax; je`），非零则 count++/idx++。
    while (idx <= 0x3e7)
    {
        // [b29-Lane21] 成员偏移两种分解实测：①`m_rows[idx].m_first`（本形）⇒ 4.4
        // `add $0xecd8; mov (%eax)`（8 折进基址常量，ORIG 为 `add $0xecd0; mov 0x8(%eax)`）；
        // ②`*(int*)((char*)&m_rows[idx] + 8)` ⇒ `add $0xecd0; add $0x8; mov (%eax)`（多 1 条）。
        // 两形各含对方缺的一半 ⇒ N1 两步常量折叠族封顶。
        if (reinterpret_cast<SigLoadEtcBestClearTimeView*>(sig)->m_rows[idx].m_first == 0)
            break;
        ++count;
        ++idx;
    }
    sig->m_bestClearTimeCount = count;
    return true;
}

// ============================================================================
// DB_LoadEtc::SendResult @0x08408766
// BigStreamPool::Acquire → guard<<userIdx<<slot→put_binary(sig,0x14b60) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
#include "DB_LoadEtc/SendResult.inc"
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x1f92),
                       true);
    guard.operator*()->operator<<(userIdx);      // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);         // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x14b60);   // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return true;                                 // ORIG ebx=1 恒定
}

// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship 见上；LoadQuestShop/_LoadQuestShop 下。
// ============================================================================

// ============================================================================
// DB_LoadEtc::_LoadQuestShop @0x08404b72（static）
// get fail 行号序列：qp/init_count 前 2 次 → cMyTrace ret0（0x17ea/0x17f1）；
// 自 max_hp 起 throw int(line)：0x17f7..0x1811、questPiece 0x1816（跳过 0x1810）。
// catch 后 memset(param_2+0xc,0,0x9a) 并 logFormat 行 0x181f。
// unroll 转录：列号用 local_14(idx) 取后自增（非字面量常数），throw 就地展开
// （__cxa_allocate_exception/__cxa_throw 逐列内联，非辅助函数），exec/fetch/
// qp/init_count 结果经 local_19(ret) 中转 → 指令数 460→690，与 ORIG 690 一致。
// [b28c-lane17] fresh 复验：690=690、助记符序列全等 ⇒ NEAR（残差全为槽号/
// callee-saved 分配差，N82 族——4.1 不按 lifetime 复用栈槽使全帧槽号错位，
// ndiff 与语义完成度反向）。按 §6.2 三件套口径判「语义完成、byte 层 N 封顶」。
// ============================================================================
#include "DB_LoadEtc/_LoadQuestShop.inc"
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
#       define QUESTSHOP_USHORT(field, line_)                                  \
        do                                                                     \
        {                                                                      \
            if (!db->get_ushort(idx++, out->field))                            \
            {                                                                  \
                throw line_;                                                   \
            }                                                                  \
        } while (0)

        QUESTSHOP_USHORT(m_data.m_maxHp, 0x17f7);                   // max_hp           +0x26
        QUESTSHOP_USHORT(m_data.m_maxMp, 0x17f8);                   // max_mp           +0x6c
        QUESTSHOP_USHORT(m_data.m_psyAttack, 0x17f9);               // psy_attack       +0x0c
        QUESTSHOP_USHORT(m_data.m_psyDefense, 0x17fa);              // psy_defense      +0x12
        QUESTSHOP_USHORT(m_data.m_magAttack, 0x17fb);               // mag_attack       +0x0e
        QUESTSHOP_USHORT(m_data.m_magDefence, 0x17fc);              // mag_defence      +0x10
        QUESTSHOP_USHORT(m_data.m_moveSpeed, 0x17fd);               // move_speed       +0x22
        QUESTSHOP_USHORT(m_data.m_attackSpeed, 0x17fe);             // attack_speed     +0x20
        QUESTSHOP_USHORT(m_data.m_hpRegen, 0x17ff);                 // hp_regen         +0x28
        QUESTSHOP_USHORT(m_data.m_mpRegen, 0x1800);                 // mp_regen         +0x46
        QUESTSHOP_USHORT(m_data.m_allElementResist, 0x1801);        // +0x3c
        QUESTSHOP_USHORT(m_data.m_fireElementResist, 0x1802);       // +0x34
        QUESTSHOP_USHORT(m_data.m_waterElementResist, 0x1803);      // +0x36
        QUESTSHOP_USHORT(m_data.m_lightElementResist, 0x1804);      // +0x3a
        QUESTSHOP_USHORT(m_data.m_darkElementResist, 0x1805);       // +0x38
        QUESTSHOP_USHORT(m_data.m_allElementAttack, 0x1806);        // +0x68
        QUESTSHOP_USHORT(m_data.m_fireElementAttack, 0x1807);       // +0x60
        QUESTSHOP_USHORT(m_data.m_waterElementAttack, 0x1808);      // +0x62
        QUESTSHOP_USHORT(m_data.m_lightElementAttack, 0x1809);      // +0x66
        QUESTSHOP_USHORT(m_data.m_darkElementAttack, 0x180a);       // +0x64
        QUESTSHOP_USHORT(m_data.m_psyCritical, 0x180b);             // +0x2a
        QUESTSHOP_USHORT(m_data.m_magCritical, 0x180c);             // +0x2c
        QUESTSHOP_USHORT(m_data.m_goodHit, 0x180d);                 // +0x4e
        QUESTSHOP_USHORT(m_data.m_evasion, 0x180e);                 // +0x42
        QUESTSHOP_USHORT(m_data.m_hitRecovery, 0x180f);             // +0x48
        QUESTSHOP_USHORT(m_data.m_separatePsyMagAttack, 0x1811);    // +0x76（跳过 0x1810）

#       undef QUESTSHOP_USHORT

        if (!db->get_uint(idx++, out->m_questPiece))            // quest_piece +0xa6
        {
            throw 0x1816;
        }
    }
    catch (int e)
    {
        memset(&out->m_data, 0, sizeof(out->m_data));
        LogManager::logFormat(1, "DBThread.cpp",
                              "static bool DB_LoadEtc::_LoadQuestShop(unsigned int,"
                              " CHARAC_QUEST_SHOP_DATA*)",
                              0x181f,
                              "DB_LoadEtc::LoadQuestShop, get(%d) ERROR charac_no=%u",
                              idx /*ORIG -0x10(%ebp)=local_14 列号*/, characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadQuestShop @0x08404a68 —— 主/tag 双实例装载。
// 主区 = SIG_LOAD_ETC+0xd5a7，tag 区 = +0xd651（tag_charac_no=+0xc 非零时装载），
// 双区齐备后 +0xd6fb 置 1。错误走 cMyTrace 0x16c4/0x16d5。
// ============================================================================
// [b28d-solo-DB_LoadEtc] 旧判「N封顶（N107 成员常量偏移取址）」已作废（b29-Lane44 推翻）：
//   ORIG 两处 `lea 0xd5a7/0xd651(%eax),%edx` 对应**视图成员取址**形；旧探针用「pad+char 数组
//   退化」形（char→CHARAC_QUEST_SHOP_DATA* 仍需 cast）故见 add+mov。改用局部 trivial 镜像
//   视图 `QuestShopView` 的具名对象成员取址（类型匹配、无 cast）后 4.4 产单条 lea ⇒ 69/69 全等。
// 已修（前波）：exec 具名 ok + trace 临时链（P227）。
#include "DB_LoadEtc/LoadQuestShop.inc"
{
    // [Lane44-新判据①] ORIG 两处均为单条 `lea 0xd5a7/0xd651(%eax),%edx`（视图成员取址）；
    // 4.4 对 `reinterpret_cast<T*>(数组名)` 恒产 `add $K,%eax; mov %eax,%edx` ⇒ 用局部
    // trivial 镜像视图成员取址复现（探针实测 4.4 产单条 lea）。
    struct QuestShopView
    {
        char                   m_pad[0xd5a7];
        CHARAC_QUEST_SHOP_DATA m_main;      // +0xd5a7
        CHARAC_QUEST_SHOP_DATA m_tag;       // +0xd651（0xd5a7+0xaa）
    };
    bool ok = _LoadQuestShop(sig->m_characNo,
                             &((QuestShopView*)sig)->m_main);
    if (!ok)
    {
        // ORIG 0x08404a90：ctor 前 m_characNo 装 %ebx 跨 ctor 保活（P227 临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x16c4, 5)(
            "DB_LoadInventory::_LoadQuestShop ERROR::charac_no(%d)",
            sig->m_characNo);
        return false;
    }

    if (sig->m_fieldC != 0)
    {
        ok = _LoadQuestShop(sig->m_fieldC,
                            &((QuestShopView*)sig)->m_tag);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x16d5, 5)(
                "DB_LoadInventory::_LoadQuestShop ERROR::tag_charac_no(%d)",
                sig->m_fieldC);
            return false;
        }
        sig->m_questShopTagLoaded = 1;           // +0xd6fb
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadTitleBook @0x08406092 —— 4 个压缩列分别解压到
// +0x466a(cap0x10ae) / +0x5718(0x2fa8) / +0x86c0(0x1c98) / +0xa358(0xe4c)。
// ============================================================================
// [b29-Lane21] **已归零（残差 0）**：四列 srcLen 改 `unsigned int` + 去 cast ⇒ ORIG
//   4 处 `mov -0x18/-0x14/-0x10/-0xc(%ebp),%edx` 装载序全复现（221=221）。
// [b28d-solo-DB_LoadEtc] N封顶（N7/N82）：ORIG 0x08406092；p167 CLEAR 21/21。
// 已修（本波）：①四段 `char blob[N] = { 0 };`（各段独立作用域声明）替 memset call ⇒
//   复现 ORIG 四段内联 rep stos（P214③/P312），p167 EXTRA memset ×2 归零；
//   ②（前波）exec 具名 ok + trace 临时链。
// 残差（不可翻）：N7 族 uncompress_zip 实参寄存器角色互换 + N82 槽序（ORIG db=-0x20/
//   ok=-0x19/srcLen=-0x18..-0xc/outLen=-0x34..-0x40 各 4 个独立槽；4.4 按 lifetime 复用
//   ⇒ db=-0x1c。全头部声明（P212）实测把 db 顶到 -0xc 反劣，已回退）。
// [b29-Lane21 复核] 三旋钮定位（ndiff 114→8，orig/our 221=221）：
//   ①`bool ok` 后置到 db 声明之后 ⇒ 槽形翻正（ORIG db=-0x20/ok=-0x19；ok 前置时 4.4 给
//     db=-0x1c/ok=-0x1d，本件方向与 P212 相反）；
//   ②四段 srcLen/outLen 声明交换（ORIG `movl $0x10ae,-0x34` 在 get_binary_length call 之后）；
//   ③fetch 检查改直测 `if (!db->fetch())`（ORIG 无 ok 槽保存，仅 exec 走 ok=-0x19）。
// 残差（P75 实参装载序，4 处）：ORIG 先取第 4 参 srcLen 再算第 1 参 dst；4.4 恒 dst 先。
#include "DB_LoadEtc/LoadTitleBook.inc"
{
    // ORIG 槽形：ok=-0x19 保存、srcLen=-0x18/-0x14/-0x10/-0xc（各列命名槽）、
    // outLen 各块独立槽 -0x34/-0x38/-0x3c/-0x40。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    bool ok;
    db->set_query("seLect general_section, specific_section, despair, event from"
                  " charac_titlebook where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x19c0, 5)(
            "LoadTitleBook(), exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())                 // ORIG 直测 call 结果（无 ok 槽保存）
    {
        return false;
    }

    // ORIG 四列各自函数级 static const compressed_buff_size（0x1410/_0 0x393c/
    // _1 0x225c/_2 0x1134），展开为四个独立作用域块保持符号一一对应。
    {
        static const int compressed_buff_size = 0x1410;
        // [b28d-solo-DB_LoadEtc] P214③：`={0}` 触发 4.4 内联 rep stos（替 memset call，N78）。
        char blob[0x1410] = { 0 };
        if (!db->get_binary(0, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen1 = db->get_binary_length(0);
        unsigned int outLen1 = 0x10ae;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookGeneral, outLen1, blob, srcLen1))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x393c;
        char blob[0x393c] = { 0 };
        if (!db->get_binary(1, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen2 = db->get_binary_length(1);
        unsigned int outLen2 = 0x2fa8;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookSpecific, outLen2, blob, srcLen2))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x225c;
        char blob[0x225c] = { 0 };
        if (!db->get_binary(2, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen3 = db->get_binary_length(2);
        unsigned int outLen3 = 0x1c98;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookDespair, outLen3, blob, srcLen3))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x1134;
        char blob[0x1134] = { 0 };
        if (!db->get_binary(3, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen4 = db->get_binary_length(3);
        unsigned int outLen4 = 0x0e4c;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookEvent, outLen4, blob, srcLen4))
        {
            return false;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadComboSkillInfo @0x0844a3b2 —— combo 区 job==9 才拉双树。
// ============================================================================
// b28d-31: N 封顶（P223 边界）——ORIG 0x0844a3b2；4.4 尾部多 [jmp;nop]
// epilogue-threading（`!call()` 等三形实测不消），语义等价。
// [b28d-solo-DB_LoadEtc] AE ndiff=0 (orig=43/ours=43)：P257 正向包裹 `if (job==9){...}`
// 消掉 4.4 的 epilogue-threading [jmp;nop] 尾岛（旧早返三形均留 1 岛）。
#include "DB_LoadEtc/LoadComboSkillInfo.inc"
{
    // ORIG: 直读 sig+0x14ab0（mov 0x14ab0(%eax)，直接折叠反偏移），无意引局部槽（P0）。
    // 局部视图使 4.4 折叠单 disp（P82/P95：数组退化 cast 会拆 add+mov）。
    struct ComboJobView
    {
        char m_pad[0x14ab0];
        unsigned int m_job;   // = m_comboSkill + 6（SIG_COMBO_SKILL::m_job，4B 装载）
    };
    // ORIG 0x0844a3c1：cmp $9/jne 直达 leave;ret（无尾岛）——试 P257 正向包裹消
    // 4.4 的 [jmp+nop] epilogue-threading 中继（旧三形均留 1 岛）。
    if (((ComboJobView*)sig)->m_job == 9)
    {
    if (!LoadComboSkill(sig, (ENUM_SKILL_TREE_KIND)0))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xbab4, "DB_LOADETC ERROR : LoadComboSkill Fail");
        sig->m_field8 = 1;
    }
    // [b28c-lane17] 判词：残余 1 条尾 `jmp+nop` 中继岛差（4.4 jump-threading，
    // P223 反例向）。实测三形：`if(!call())` 43/44（最优）、
    // `if(call()==0)` 45、`if(positive){}else{}` 45 —— 均无法消该岛，
    // 保留最短残差形。
    if (!LoadComboSkill(sig, (ENUM_SKILL_TREE_KIND)1))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xbaba, "DB_LOADETC ERROR : LoadComboSkill Second Fail");
        sig->m_field8 = 1;
    }
    }
}


// ============================================================================
// DB_LoadEtc::LoadComboSkill @0x0844a47a —— 单树连招装载（kind∈{0,1}）。
// ============================================================================
// [b29-Lane21] 残差 6 行 = N76 store 装载序：ORIG `mov -0xc(%ebp),%eax; mov %eax,%edx;
//   mov 0xc(%ebp),%eax; mov %dl,disp`（4 条，RHS-first + 中转）vs 4.4 `mov -0xc(%ebp),%edx;
//   mov 0xc(%ebp),%eax; mov %dl,disp`（3 条）；视图/成员/两值形已试 ⇒ N76 封顶。
// [b28d-solo-DB_LoadEtc] N封顶（N76 store 装载序）：ORIG 0x0844a47a。
// 已修（本波）：①**真 bug（P92/P196）**：getComboSkill 第 2 实参原写 sig->m_accId（+0），
//   ORIG 0x0844a5a4 为 `mov 0x4(%eax),%eax` = sig->m_characNo（+4）——该值即
//   getComboSkill 日志的 charac_no=%u，原码打印错误账号。
//   ②去 cs 具名局部（P183）+ 改成员视图 ComboSkillView（pad 0x14aae + tree1Count/tree2Count/
//   tree1Data[6]/tree2Data[6]）⇒ ORIG disp 形 `mov %dl,0x14aae(%eax)` / `lea 0x14ab4(%eax),%edx`
//   （cast 视图恒 `add $0x14aaa` + disp4，N107）。
//   ③四段改 **if / else-if** 链（count 写与两处调用各自成对）⇒ 复现 ORIG 的 `jmp` 块尾 +
//   `cmp $1` elif 布局（全独立 if 时 4.4 直落无 jmp）。
// 残差（不可翻，N76）：count 写点 store 装载序——ORIG `mov -0xc,%eax; mov %eax,%edx;
//   mov sig,%eax; mov %dl,disp`（RHS-first + eax→edx 中转）vs 4.4 恒 LHS-first 直存
//   `mov sig,%eax; mov -0xc,%edx; mov %dl,disp`；字段 unsigned char 与 int RHS 本已异型，
//   P211 两旋钮（字段/callee）无第三档可用。
//   [b28d-solo 复测] 去 `(unsigned char)` cast（`m_tree1Count = nRows;`）产物**完全相同**
//   （4.4 视 uchar←int 为可直存，P211 档③不适用）⇒ 维持去 cast 形；p167 CLEAR 15/15。
// 已修（前波）：去 kindVal 局部（P0 直比形参）+ exec 具名 ok + 四处 trace 临时链。
#include "DB_LoadEtc/LoadComboSkill.inc"
{
    // ORIG 0x0844a47a：kind 无具名槽（test/jne + cmp $1/je 直比形参，P0 槽律）。
    if (!(kind == 0 || kind == 1))
    {
        cMyTrace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                 " ENUM_SKILL_TREE_KIND)", 0xbac4, 5)(
            "DB_LoadEtc::LoadComboSkill kind(%d) error characNo(%u)", kind,
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
    bool ok = db->exec(true);                                // ORIG -0xd 具名槽
    if (!ok)
    {
        cMyTrace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                 " ENUM_SKILL_TREE_KIND)", 0xbad4, 5)(
            "DB_LoadEtc::LoadComboSkill, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }

    // 树条目计数写回：kind0→+0x14aae（tree1Count）、kind1→+0x14aaf；
    // ORIG 对两 kind 各自独立调用 getComboSkill（0x844a5b2 / 0x844a614）。
    // 真 bug 修正（P92/P196）：ORIG 0x0844a5a4 实参是 `mov 0x4(%eax),%eax` = sig->m_characNo，
    // 原写 sig->m_accId（+0）⇒ getComboSkill 日志 charac_no 打印错误值。
    // P183：无 cs 具名局部（ORIG 每次重发 sig+0x14aaa 全链，槽 -0x14 起不漂移）。
    // 成员视图直接命中 ORIG 的 disp 形（`mov %dl,0x14aae(%eax)`），避免 cast 视图
    // `add $0x14aaa,%eax` + disp4（N107）。
    // [b29-Lane21 实测] Main「视图 cast 反模式」洞察在本件**反向**：改权威类型
    // `reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)->m_tree1Count = nRows;`
    // ⇒ 4.4 产 `add $0x14aaa,%eax; mov -0xc,%edx; mov %dl,0x4(%eax)`（disp 被拆成两段），
    // ndiff 6→18 反劣，已回退（本件真差异是 store 装载序 LHS/RHS，非 disp/N107）。
    struct ComboSkillView
    {
        char m_pad[0x14aae];
        unsigned char m_tree1Count;      // +0x14aae（N76：P211 档③不适用——字段与
        unsigned char m_tree2Count;      // +0x14aaf  RHS 本已异型，signed/unsigned 同形）
        char m_pad2[4];                  // +0x14ab0
        comoboSkillData m_tree1Data[6];  // +0x14ab4（0x54）
        comoboSkillData m_tree2Data[6];  // +0x14b08（0x54）
    };
    if (kind == 0)
    {
        // ORIG 0x0844a5c7：`mov -0xc(%ebp),%eax; mov %eax,%edx; mov 0xc(%ebp),%eax;
        //   mov %dl,0x14aae(%eax)`（RHS-first + eax→edx 中转，N76 封顶）。
        ((ComboSkillView*)sig)->m_tree1Count = nRows;
    }
    else if (kind == 1)
    {
        ((ComboSkillView*)sig)->m_tree2Count = nRows;      // 同上（ORIG 0x0844a5e0）
    }
    if (kind == 0)
    {
        if (!getComboSkill(db, sig->m_characNo, nRows,
                ((ComboSkillView*)sig)->m_tree1Data))
        {
            cMyTrace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                     " ENUM_SKILL_TREE_KIND)", 0xbae7, 5)(
                "DB_LoadEtc::LoadComboSkill, getComboSkill ERROR charac_no=%u",
                sig->m_characNo);
        }
    }
    else if (kind == 1)
    {
        if (!getComboSkill(db, sig->m_characNo, nRows,
                ((ComboSkillView*)sig)->m_tree2Data))
        {
            cMyTrace("bool DB_LoadEtc::LoadComboSkill(SIG_LOAD_ETC*,"
                     " ENUM_SKILL_TREE_KIND)", 0xbaef, 5)(
                "DB_LoadEtc::LoadComboSkill, getComboSkill ERROR charac_no=%u",
                sig->m_characNo);
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::getComboSkill @0x0844a704 —— 每行 7 个 short 填一个 comoboSkillData，
// 行数上限 min(count,6)；成功返回 1、任何 get 失败行号错误路径返回 0。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x0844a704（orig=124/ours=124，槽全等）。
// 已修（本波）：①循环改标准 `for (row = 0; row < count && row < 6; ++row)` + 尾 `return true;`
//   ⇒ 4.4 复现 ORIG 的「入口 jmp 底部条件块 + 短路物化 mov$1/$0」（P283/P214①）；
//   原 `for(;;)+if(!(…))return true` 使条件置顶（DIFF）。
//   ②去 slot 具名指针局部，且**关键**：地址式由 `(comoboSkillData*)((char*)out + row*0xe)`
//   改**数组下标 `out[row]`**（sizeof(comoboSkillData)==0xe，pack(1)）——指针 cast 形被 4.4
//   缓存到 -0x14 临时槽（row 被挤到 -0x18、三处 trace 槽整体浅 4）；下标形每次重算
//   地址（P183 地址重物化），row=-0x14/col=-0x10/k=-0xc/trace -0x44/-0x34/-0x24 全等。
//   ③k 循环 `k <= 5`（ORIG 尾部 setle 形，P51）。
// 已修（前波）：col 改 0 起取后自增（ORIG -0x10）+ 三处 trace 临时链。
#include "DB_LoadEtc/getComboSkill.inc"
                               comoboSkillData* out)
{
    if (db == 0 || out == 0)
    {
        return false;
    }
    for (int row = 0; row < count && row < 6; ++row)
    {
        if (!db->fetch())
        {
            cMyTrace("bool DB_LoadEtc::getComboSkill(MySQL*, unsigned int,"
                     " int, comoboSkillData*)", 0xbb01, 5)(
                "DB_LoadEtc::getComboSkill fetch, exec() ERROR charac_no=%u",
                accId);
            return false;
        }
        // P183 地址重物化：ORIG 每次重发 row*0xe 全链（无 slot 具名局部缓存）。
        int col = 0;                                          // ORIG -0x10 起 0，取后自增
        if (!db->get_short(col++, out[row].m_comboSkillIndex))
        {
            cMyTrace("bool DB_LoadEtc::getComboSkill(MySQL*, unsigned int,"
                     " int, comoboSkillData*)", 0xbb0a, 5)(
                "DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u", col,
                accId);
            return false;
        }
        for (int k = 0; k <= 5; ++k)
        {
            if (!db->get_short(col++, out[row].m_comboSkillList[k]))
            {
                cMyTrace("bool DB_LoadEtc::getComboSkill(MySQL*,"
                         " unsigned int, int, comoboSkillData*)", 0xbb14, 5)(
                    "DB_LoadEtc::getComboSkill get(%d) ERROR charac_no=%u",
                    col, accId);
                return false;
            }
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::GetUserSkill @0x084075d2 —— 主 skill 行 + tag 双槽位 blob。
// §6.2 语义收口：12 列 get/blob 链、col 槽（-0x10）col++ 形、fail 岛行号
// （0x1d50..0x1df1）与 ORIG 对齐；AE 天花板=N82 trace 槽方向三证（Main
// batch-17 裁定按语义收口，不投纯文本序差）。
// 列序 12 列，目标偏移照汇编；n_rows==0 且 PvP 技能树频道时补插一行。
// 错误行号 0x1d50/0x1d5d/0x1d69/0x1d71/0x1d98/0x1dac/0x1db2/0x1dea/0x1df1。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x084075d2；p167 CLEAR（0/0），
// 残差为帧/槽分配（ORIG 478 vs 4.4 468）。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=624 (orig=478/ours=468)：N82 帧/槽错位（12 列 get/blob
// 链的 trace 槽方向）+ N89 镜像（get_compressed_blob_data 内联差）；p167 CLEAR 0/0 ⇒ 语义完成。
// [b28d-SweepDBLoadEtc 本波] 真 bug 修复：①col 初值 1→0（ORIG 0x084077e9 `movl $0x0,-0x10`
// + 首列 `get_uint(col++)`；旧 col=1 使后续 blob 列号整体 +1）；②去 sk 指针/spVals 局部（P183，
// ORIG 每处单条 lea/add disp 直写成员，改 m_mixedRegion[off] 常量索引）；③scriptVersion 直传
// 成员地址（ORIG `lea 0xcdd1(%eax),%edx`）；④tag 段 col 变量（ORIG -0xc）；⑤9 处 trace 改
// P227 全表达式临时链；⑥exec/fetch/get_uint 共 9 处改具名 ok 槽（ORIG -0x15）。
// 读数：gap +10（478/468）→ +1（478/477）；p167 46=46（仅 cMyTrace C1/C2 别名）；
// 残差=add+mov↔lea 跨版本（N107/P30）。
// [b29-Lane44 判据①复扫·已归零] 上记 9 处 add+mov↔lea 实为**可翻**（旧判 N107 系探针形不全）：
//   4.4 对「cast 取址 `(T*)&arr[i]`」恒产 `add $K,%eax; mov %eax,%edx`，对「视图成员/
//   指针算术/数组名退化」恒产单条 `lea K(%eax),%edx`；4.1 恰好相反 ⇒ 两侧互补，改我方表达式
//   形即可逐点对齐（探针 /tmp/bl_probe.cpp~bl_probe5.cpp，447/446 后端同形）：
//   ①ORIG lea 的 4 点（+0xc9c8/+0xcb64/+0xcd00/+0xcd04）→ 局部 trivial 镜像视图
//     `SkillMixedView` 具名成员取址（类型匹配、无 cast）；
//   ②ORIG `add K; mov %eax,%edx` 的 5 点（+0xc9cc/+0xcb68/+0xcd08/+0x29fd/+0x2b97，均为
//     数组名退化/指针算术）→ 整型加法形 `(char*)((unsigned int)sig + K)`（P319 同族，
//     32 位指针≡int，语义等价）。
//   读数：478/477（ndiff 18，9 hunk）→ **478/478 全等**（verdict AE 54，TU diff 12→11）。
#include "DB_LoadEtc/GetUserSkill.inc"
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc12GetUserSkill…E9handleIdx
    bool ok;                           // ORIG -0x15 具名槽（深于 db=-0x14，P212）
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect remain_sp,skill_slot,remain_sp_2nd, skill_slot_2nd,"
                  " remain_sfp_1st, remain_sfp_2nd, skill_command, skill_slot_lethe,"
                  " lethe_flag, skill_slot_lethe_2nd, lethe_flag_2nd,script_version"
                  " from %s where charac_no=%u",
                  get_skill_table_name(false), sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d50, 5)(
            "DB_LoadSkill::GetUserSkill, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    if (db->get_n_rows() == 0 && G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG：缺行 + PvP 技能树频道 → inSert 占位行并置 0xcdd4 标志。
        db->set_query("inSert into %s (charac_no) values(%u)",
                      get_skill_table_name(false), sig->m_characNo);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1d5d, 5)(
                "DB_LoadSkill::GetUserSkill insert, exec() ERROR"
                " charac_no=%u", sig->m_characNo);
            return false;
        }
        sig->m_pvpSkillInsertFlag = 1;                       // +0xcdd4 movb
        return true;
    }

    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d69, 5)(
            "DB_LoadSkill::GetUserSkill, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    int col = 0;                                             // ORIG -0x10 从 0 起
    // 混合区首 0x414 为 SIG_LOAD_SKILL 视图（+0xc9c0；字段名见 SigTypes_C.h，
    // 证据 GetSkill.md/SkillSlot 透传布局：+0x8/+0xc/+0x1a4/+0x1a8/+0x340/
    // +0x344/+0x348 = sig+0xc9c8/0xc9cc/0xcb64/0xcb68/0xcd00/0xcd04/0xcd08）。
    // P183/P30：ORIG 每处单条 lea/add disp（+0xc9c8/+0xc9cc/... 折叠），禁 sk 指针
    // 缓存、禁 (SIG_LOAD_SKILL*)sig->m_mixedRegion->field（4.4 拆 add $0xc9c0;add $disp）。
    // 常量索引与 SIG_LOAD_SKILL 视图字段一一对应（+0x8/+0xc/+0x1a4/+0x1a8/+0x340/
    // +0x344/+0x348）。
    // [Lane44-新判据①] ORIG 0xc9c8/0xcb64/0xcd00/0xcd04 为单条 `lea K(%eax),%edx`
    //   （视图成员取址形），4.4 对 `(unsigned int*)&arr[i]` 恒产 `add $K,%eax; mov`⇒ 改用
    //   局部 trivial 镜像视图成员取址（探针实测 4.4 产单条 lea），四字段偏移见下。
    struct SkillMixedView
    {
        char           m_pad[0xc9c8];
        unsigned int   m_remainSp;         // +0xc9c8（ORIG lea 形）
        char           m_pad2[0x198];      // +0xc9cc..0xcb64
        unsigned int   m_remainSp2nd;      // +0xcb64（ORIG lea 形）
        char           m_pad3[0x198];      // +0xcb68..0xcd00
        unsigned int   m_field340;         // +0xcd00（ORIG lea 形）
        unsigned int   m_field344;         // +0xcd04（ORIG lea 形）
    };
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_remainSp);
    if (!ok)
    {                                                        // col0 remain_sp
        cMyTrace(__PRETTY_FUNCTION__, 0x1d71, 5)(
            "DB_LoadSkill::GetUserSkill, get(0) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0xc9cc),
                                  0x198, 2))
    {                                                        // skill_slot
        return false;
    }
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_remainSp2nd);
    if (!ok)
    {                                                        // remain_sp_2nd→0xcb64
        cMyTrace(__PRETTY_FUNCTION__, 0x1d98, 5)(
            "DB_LoadSkill::GetUserSkill, get(4) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0xcb68),
                                  0x198, 2))
    {                                                        // skill_slot_2nd
        return false;
    }
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_field340);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dac, 5)(
            "DB_LoadSkill::GetUserSkill, get(7) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++,
                      ((SkillMixedView*)sig)->m_field344);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1db2, 5)(
            "DB_LoadSkill::GetUserSkill, get(8) ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0xcd08),
                                  200, 4))
    {                                                        // skill_command
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0x29fd), 0x198, 2))
    {                                                        // skill_slot_lethe
        return false;
    }
    int letheFlag = 0;                                       // ORIG -0xac 列8
    if (!db->get_int(col++, letheFlag))                      // lethe_flag(int)
    {
        return false;
    }
    if (!get_compressed_blob_data(db, col++,
                                  (char*)((unsigned int)sig + 0x2b97), 0x198, 2))
    {                                                        // skill_slot_lethe_2nd
        return false;
    }
    int letheFlag2 = 0;                                      // ORIG -0xb0 列10
    if (!db->get_int(col++, letheFlag2))                     // lethe_flag_2nd(int)
    {
        return false;
    }
    if (!db->get_ubyte(col++, sig->m_scriptVersion))         // script_version→+0xcdd1
    {
        return false;
    }

    // lethe 标志低位字节落位（ORIG movzbl/mov dl）。
    sig->m_backupSkill[0x198] = (unsigned char)letheFlag;            // +0x2b95
    sig->m_oneDayLetheFlag = (unsigned char)letheFlag;               // +0x29fc
    sig->m_backupSkill2ND[0x198] = (unsigned char)letheFlag2;        // +0x2d2f
    sig->m_oneDayLetheFlag2ND = (unsigned char)letheFlag2;           // +0x2b96

    if (sig->m_fieldC != 0)
    {
        // tagCharacNo 存在 → 拉主/tag 双槽位 blob。
        db->set_query("seLect skill_slot, skill_slot_2nd from skill where"
                      " charac_no=%u", sig->m_fieldC);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1dea, 5)(
                "DB_LoadSkill::GetUserSkill, exec() ERROR tagCharacNo=%u",
                sig->m_fieldC);
            return false;
        }
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1df1, 5)(
                "DB_LoadSkill::GetUserSkill, fetch() ERROR tagCharacNo=%u",
                sig->m_fieldC);
            return false;
        }
        int col = 0;                                        // ORIG -0xc 块内槽
        if (!get_compressed_blob_data(db, col++,
                                      reinterpret_cast<char*>(&sig->m_tagEtc),
                                      0x198, 2))
        {
            return false;
        }
        if (!get_compressed_blob_data(db, col++,
                                      reinterpret_cast<char*>(sig->m_tagEtc.m_skills[1]),
                                      0x198, 2))
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
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x08445660（orig=151/ours=151，槽全等）。
// 已修（本波）：①`bool ok;` 声明前置于 db（ORIG ok=-0x11 深于 db=-0x10，P212）；
//   ②三处 get 结果复用同一 ok 槽（ORIG 三处 `mov %al,-0x11; movzbl; xor $1; test`；
//   原 get_binary/get_compressed 直测少 2 条槽往返）。
// 已修（N107 翻正）：cooltime 实参 `sig->m_cooltimeItem`（数组名退化）4.4 恒
//   `add $0x230c,%eax; mov %eax,%edx`；改 **`&sig->m_cooltimeItem[0]`**（显式取首元素地址）
//   即复现 ORIG `lea 0x230c(%eax),%edx`——N107 的可翻位（与 effect 的 `&m_effectItem[0]` 同形）。
// 已修（前波）：exec/fetch 具名 ok + col 槽取后自增 + 三处 trace 临时链。
#include "DB_LoadEtc/LoadCharacItemStat.inc"
{
    // ORIG 槽序：ok(-0x11) 深于 db(-0x10) ⇒ 原码 ok 先声明（P212「先声明深」）；
    // 三处 get 结果均复用该槽（exec/fetch/get_binary/get_compressed）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect cooltime_item, effect_item, check_flag from"
                  " charac_item_stat where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);                                     // ORIG -0x11 具名槽
    if (!ok)
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    ok = db->fetch();
    if (!ok)
    {
        return false;
    }

    int col = 0;                                             // ORIG -0xc 列号槽
    ok = db->get_binary(col++, &sig->m_cooltimeItem[0], 0x194);  // cooltime_item +0x230c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xac8a, 5)(
            "DB_LoadEtc::LoadCharacItemStat cooltime, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    ok = db->get_binary(col++, &sig->m_effectItem[0], 0x194);    // effect_item +0x24a4
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xac91, 5)(
            "DB_LoadEtc::LoadCharacItemStat effect_item, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    ok = get_compressed_blob_data(db, col++, sig->m_itemDictionary, 5000, 0);
    if (!ok)
    {                                                        // check_flag
        cMyTrace(__PRETTY_FUNCTION__, 0xac98, 5)(
            "DB_LoadEtc::LoadCharacItemStat item_dictionary, get(%d) ERROR"
            " charac_no=%u", col, sig->m_characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadItemMakingSkill @0x084040b4 —— 11 列 short 循环装载，
// 目标基址 sig+0x10+(col+0x6b78)*2（=0xD700+col*2）。错行号 0x15dd/0x15e8/0x15f2。
// ============================================================================
// [b28d-solo-DB_LoadEtc] N封顶（N1 常量折叠 + 求值序）：ORIG 0x084040b4。
// 已修（本波）：①`bool ok;` 声明前置于 db ⇒ 槽序对齐（ok=-0x11 深于 db=-0x10，帧 0x54；
//   P212「先声明深」）；②地址式改 offset-0 视图 `m_rows[0x6b80 + col]`（元素基址即 sig）。
// 残差（不可翻，N1）：ORIG `mov col,%eax; add $0x6b78,%eax; add %eax,%eax; add sig,%eax;
//   add $0x10,%eax`（index 先、base 用内存操作数、+0x10 独立）vs 4.4
//   `mov sig,%eax; mov col,%edx; add $0x6b80,%edx; add %edx,%edx; add %edx,%eax`
//   ——4.4 恒把基址 +0x10 折进索引常量（N1）。
//   [b28d-solo 复测] 补测第五形：视图 `{char m_pad[0x10]; short m_rows[0x6b78+11];}` +
//   `m_rows[0x6b78 + col]` ⇒ 4.4 仍折成 `add $0x6b78,%edx; add $0x8,%edx`（+0x10/2 折入
//   索引），118 vs 119 条；视图形/short/char/指针左结合四形 + 本形共五形均折 ⇒ 封顶。
// 已修（前波）：exec/fetch/get_short 复用具名 ok + trace 临时链 + **真 bug 修正**（exec 日志参数
//   原写 m_accId，ORIG 0x084040e4 为 +0x4 = m_characNo，P92 来源照抄）。
#include "DB_LoadEtc/LoadItemMakingSkill.inc"
{
    // ORIG 槽序：ok(-0x11) 深于 db(-0x10) ⇒ 原码 ok 先声明（P212「先声明深」）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect weapon, cloth, leather, light_armor, heavy_armor, plate,"
                  " amulet, wrist, ring, support, magic_stone from"
                  " item_making_skill_info where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);                                     // ORIG -0x11 具名槽（exec/fetch 复用）
    if (!ok)
    {
        // ORIG 0x084040e4：ctor 前把 m_accId 装 %ebx 跨 ctor 保活 ⇒ P227 临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x15dd, 5)(
            "DB_LoadEtc::LoadItemMakingSkill, exec() ERROR charac_no=%u",
            sig->m_characNo);            // ORIG +0x4 = characNo（P92 日志参数来源照抄）
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x15e8, 5)(
            "DB_LoadEtc::LoadItemMakingSkill fetch, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    for (int col = 0; col <= 0xa; ++col)                     // cmpl $0xa setle → ≤10
    {
        // ORIG 地址式 0x084041f0：`mov -0xc(%ebp),%eax; add $0x6b78,%eax;
        //   add %eax,%eax; add 0xc(%ebp),%eax; add $0x10,%eax`（index 先、base 后、
        //   成员偏移 +0x10 独立一条，共 5 条）⇒ 等价 +0xd700 + col*2。
        // [K2ADBLogin 2026-09-11] 改 **完整类型成员直访问** `sig->m_itemMakingSkill[col]`
        //   （P235/§4.3 真实成员优先于自造视图）后 base-first 反序翻正为 index-first，
        //   读数 118/118 → ours 117（旧指针 cast 视图形为 base-first 5 行失配）。
        // 残差：`add $0x6b80`（0x10 折进索引常量）vs ORIG `add $0x6b78`+`add $0x10`
        //   两步 ⇒ N1 两步常量折叠族（4.4 代数化简不可关；已试元素型数组/双视图 4 形）。
        ok = db->get_short(col, reinterpret_cast<short&>(sig->m_itemMakingSkill[col]));
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x15f2, 5)(
                "DB_LoadEtc::LoadItemMakingSkill get(%d) ERROR charac_no=%u",
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
// [b28d-solo-DB_LoadEtc] **AE（残差 0；232=232、p167 CLEAR 24/24）**：ORIG 0x08406eec。
// 已修（本波，本件归零）：`sig->m_todLastClearLayer = layer.m_layer` 改**具名视图 cast**
//   `reinterpret_cast<TODLayerSlotView*>(sig)->m_layer = layer.m_layer`（探针
//   /tmp/dble_probe/p1.cpp f7）⇒ 复现 ORIG 0x08407114 的 base-first 装载序
//   （`mov 0xc(%ebp),%eax` 先于 `movzwl -0x78(%ebp),%edx`）；直写具名字段 4.4 恒 RHS-first
//   （旧「N107 + P211 不可翻」判词作废）。位模式/movw 宽度不变，语义等价。
// 已修（前波）：①六处 trace 改 **P227 全表达式临时链** `cMyTrace(...)("fmt", sig->m_characNo)`
//   ⇒ trace 槽方向翻正（ORIG -0x74/-0x64/-0x54/-0x44/-0x34/-0x24 全等；具名 tt 形恒
//   反向 -0x28..-0x78 且 m_characNo 在 ctor 后才载）+ m_characNo 装 %ebx 跨 ctor 保活；
//   ②`unsigned short layerVal;` 去预清（ORIG 无 `movw $0,-0x76`，get_ushort 直写）；
//   ③末列改 `get_int(col++, …)`（ORIG 0x0840706x 有 `addl $1,-0x10`）。
// 残差：无（`lea 0xd72c(%eax),%edx` 取址形两侧本就一致；p167 CLEAR 24/24）。
// 已修（前波）：三守卫改正极性嵌套（exec/fetch/AffectedRowCount==1 共享尾 return true）。
#include "DB_LoadEtc/LoadTODUserState.inc"
{
    static const int handleIdx = 2;    // ORIG _ZZN10DB_LoadEtc16LoadTODUserState…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect unix_timestamp(first_layer_start_date),"
                  "today_enter_count,last_clear_layer,enter_count_by_week,"
                  "unix_timestamp(m_date),unix_timestamp(last_clear_date) from"
                  " charac_tower_despair where charac_no = %u", sig->m_characNo);
    // ORIG 0x08406f04 起：exec/fetch/AffectedRowCount==1 三守卫**正极性**嵌套
    // （bool 直测 test/je + `or/xor/sete`），任一失败共享函数尾 return true
    // （P214①/P59；负极性形多 xor/mov$1/jmp 块）。
    if (db->exec(true))
    {
    if (db->fetch())
    {
    if (db->getAffectedRowCount() == 1)
    {
    int col = 0;                          // ORIG 08406f85 movl $0,-0x10
    bool ok = false;                      // ORIG 08406f8c movb $0,-0x9 每步 get 同槽保存
    ok = db->get_int(col++, sig->m_todFirstLayerStartDate);  // +0xd71c
    if (!ok)
    {                                                        // first_layer_start_date
        // P227 全表达式临时链（ORIG 0x08406fbd：ctor 前 m_characNo 装 %ebx 跨 ctor 保活；
        // 具名 tt 形使 trace 槽方向反 + 值在 ctor 后才载）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1b77, 5)(
            "TOD : select from charac_tower_despair charac_no=%u",
            sig->m_characNo);
        return false;
    }
    ok = db->get_int(col++, sig->m_todTodayEnterCount);      // +0xd720
    if (!ok)
    {                                                        // today_enter_count
        cMyTrace(__PRETTY_FUNCTION__, 0x1b7f, 5)(
            "TOD select from charac_tower_despair charac_no=%u",
            sig->m_characNo);
        return false;
    }
    unsigned short layerVal;             // ORIG 无预清（get_ushort 直写 -0x76）
    ok = db->get_ushort(col++, layerVal);                    // last_clear_layer(ushort)
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b88, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    TOD_Layer layer(layerVal);                               // ORIG TOD_Layer C1(T)@0x085fe7b4
    // [b28d-solo-DB_LoadEtc] P211 同型化：ORIG 0x08407114 为 base-first
    // （`mov 0xc(%ebp),%eax` 先于 `movzwl -0x78(%ebp),%edx`）；直写具名字段
    // 4.4 恒 RHS-first（探针 /tmp/dble_probe/p1.cpp f1）。具名视图 cast（同探针
    // f7）复现 ORIG 的 base-first + 单 disp `mov %dx,0xd724(%eax)`。
    struct TODLayerSlotView
    {
        char m_pad[0xd724];
        unsigned short m_layer;
    };
    reinterpret_cast<TODLayerSlotView*>(sig)->m_layer = layer.m_layer;  // +0xd724（movw）
    ok = db->get_short(col++,
                       *reinterpret_cast<short*>(&sig->m_todEnterCountByWeek));
    if (!ok)
    {                                                        // enter_count_by_week
        cMyTrace(__PRETTY_FUNCTION__, 0x1b92, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    ok = db->get_int(col++, sig->m_todDate);                 // +0xd728
    if (!ok)
    {                                                        // m_date
        cMyTrace(__PRETTY_FUNCTION__, 0x1b9a, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    ok = db->get_int(col++, sig->m_todLastClearDate);        // +0xd72c（ORIG addl $1 实证）
    if (!ok)
    {                                                        // last_clear_date
        cMyTrace(__PRETTY_FUNCTION__, 0x1ba2, 5)(
            "[@TOD] select from charac_tower_despair charac_no=%d",
            sig->m_characNo);
        return false;
    }
    }
    }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadVillageAttackInfo @0x0843f038 —— 按重置基准日统计当日
// attack_count/revenge_dungeon（+0xc9ac/+0xc9ad）；无记录两字节清零且 ret 0。
// ============================================================================
#include "DB_LoadEtc/LoadVillageAttackInfo.inc"
{
    if (sig->m_characNo == 0)
    {
        return false;
    }

    long cur = GlobalData::s_systemTime_.getCurSec();        // ORIG localtime 单线程域
    struct tm* ptm = localtime(&cur);
    time_t baseDay;                                          // ORIG 槽 -0x18 位次
    int fatigueHour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // +0x37c
    if (ptm->tm_hour < fatigueHour)
    {
        ptm = 0;
        cur -= 0x15180;
        ptm = localtime(&cur);
    }
    baseDay = mktime(ptm);
    (void)baseDay;

    char occDate[12];
    memset(occDate, 0, sizeof(occDate));
    sprintf(occDate, "%04d-%02d-%02d",
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
    int fetched = db->fetch();                              // ORIG movzbl %al → int 槽
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
// [b28d-solo-DB_LoadEtc] NEAR（N7 封顶）：ORIG 0x08405c6e。
// 残差（不可翻，N7/4.4.7）：间接虚调寄存器角色互换——ORIG
//   `mov (%eax),%edx; mov -0x14(%ebp),%eax; mov %eax,(%esp); call *%edx`
//   vs 4.4.7 `mov (%eax),%eax; mov -0x14(%ebp),%edx; …; call *%eax`
//   （P230 补：4.4.6 后端同 ORIG 用 %edx，本 TU 未路由 446 表 ⇒ 4.4.7 恒 %eax；改路由属 Main）。
// 已修：首段链式去 evMng 槽、三处 get_convert 重发（P48）、nRows 正极性 !=0 直落（P220）、
//   set_query 内联实参与 NumberToString（P259）。助记符序列全等。
#include "DB_LoadEtc/LoadAccountFirstLogin.inc"
{
    // ORIG 0x08405c75：首段 is_eventing 为链式直调（无 evMng 具名槽，P0 槽律）。
    if (!reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->is_eventing(eventCode))
    {
        return false;
    }
    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* entity = evMng->get_event_entity(eventCode);
    if (entity == 0)
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    // ORIG vtable+0x20 虚调用 get_db_table_name → &m_dbTableName（std::string 视图）
    // 经 toTString(std::string const&) 取表名；accStr 无具名槽（ebx 跨调用保活）。
    db->set_query("seLect m_id from %s where m_id=%s",
                  toTString(entity->get_db_table_name()),
                  NumberToString(sig->m_accId, 0));

    if (!db->exec(true))
    {
        // ORIG 0x08405d5d/0x08405da7/0x08405dd8：三处各自重发 get_convert 调用（P48 无 CSE）。
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);
        return false;
    }
    int nRows = db->get_n_rows();                            // ORIG -0xc 具名槽
    // ORIG 0x08405d9e：cmpl $0,-0xc; je <后置 reset 块> ⇒ 正极性 !=0 的 set 块直落。
    if (nRows != 0)
    {
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);  // ORIG bitset::set(Ejb)
    }
    else
    {
        sig->m_bitset.reset((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode));  // ORIG bitset::reset(Ej)
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAccountFirstLoginDaily @0x08405e0a —— 日频首登事件。
// ============================================================================
// [b28d-solo-DB_LoadEtc] NEAR（N7 封顶）：ORIG 0x08405e0a。
// 已修（本波）：①去 `bool ok = db->exec(true)` 具名槽改直测 `if (!db->exec(true))`
//   ⇒ ok 原占 -0xd，把 baseSec 挤到 -0x14 并令全槽组（evMng/entity/today/db/nowT/curTm/
//   resetTm）整体深 4；删除后逐槽回正（-0x20/-0x1c/-0x18/-0x14/-0x24/-0x50/-0x7c）。
//   ②nRows 分支改正极性 `if (nRows != 0) { set } else { reset }`（ORIG `je <reset块>`，P220）。
// 残差（不可翻，N7/4.4.7）：虚调 get_db_table_name 的目标寄存器——ORIG `mov (%eax),%edx; …
//   call *%edx` vs 4.4.7 `mov (%eax),%eax; …; call *%eax`（本 TU 未路由 446，P230 补）。
// 已修（前波）：首段链式、today 走链式、entity 用 evMng 槽。
#include "DB_LoadEtc/LoadAccountFirstLoginDaily.inc"
{
    // b28d-31: ORIG 0x08405e0a —— 首段链式（无 evMng 具名槽）；第二段具名 -0x20。
    // get_convert_to_user_confirmflag 在 3 处 set/reset 各重发（P167 差集 ×2）。
    if (!reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->is_eventing(eventCode))
    {
        return false;
    }
    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* entity = evMng->get_event_entity(eventCode);
    if (entity == 0)
    {
        return false;
    }

    // ORIG 0x08405ebc：today 走重新链式（无 evMng 槽复用）。
    int today = reinterpret_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())->get_today_time();
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
    // ORIG 0x08405f2c：11 个字段逐对 `mov src,%eax; mov %eax,dst`（P54 逐字段赋值形）。
    resetTm.tm_sec = curTm.tm_sec;
    resetTm.tm_min = curTm.tm_min;
    resetTm.tm_hour = curTm.tm_hour;
    resetTm.tm_mday = curTm.tm_mday;
    resetTm.tm_mon = curTm.tm_mon;
    resetTm.tm_year = curTm.tm_year;
    resetTm.tm_wday = curTm.tm_wday;
    resetTm.tm_yday = curTm.tm_yday;
    resetTm.tm_isdst = curTm.tm_isdst;
    resetTm.tm_gmtoff = curTm.tm_gmtoff;
    resetTm.tm_zone = curTm.tm_zone;
    resetTm.tm_hour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;       // ORIG tm_hour=+0x37c
    resetTm.tm_min = 0;
    resetTm.tm_sec = 0;
    long baseSec = mktime(&resetTm);
    if (curTm.tm_hour < resetTm.tm_hour)
    {
        baseSec -= 0x15180;
    }

    db->set_query("seLect m_id from %s where m_id=%u and occ_date >"
                  " FROM_UNIXTIME(%d)", toTString(entity->get_db_table_name()),
                  sig->m_accId, baseSec);

    // ORIG 0x08405fd5：exec 结果直测（无 ok 具名槽）⇒ -0xd 归 baseSec，全槽组回正 4B。
    if (!db->exec(true))
    {
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);
        return false;
    }
    int nRows = db->get_n_rows();                            // ORIG -0xc 具名槽
    // ORIG 0x08406025：cmpl $0,-0xc; je <reset 块> ⇒ set 块直落（正极性 !=0，P220）。
    if (nRows != 0)
    {
        sig->m_bitset.set((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode), true);
    }
    else
    {
        sig->m_bitset.reset((unsigned int)
            evMng->get_convert_to_user_confirmflag(eventCode));
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadFairPvPScore @0x08404290 —— fair_pvp_score 14 列；
// n_rows==0 时补插并返回；give_item 压缩列（cap 0x650）→ +0xcf4b；
// daily 重置比较（last_play_time < getDailyInitializeTime(6,0,0) → +0xd59b=0）。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x08404290；474 vs 476（帧/寄存器分配），
// p167 待核。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=516 (orig=474/ours=476)：N82 帧/槽错位（14 列 + 13 处
// trace 的复用 bool ok 槽）；p167 CLEAR 0/0 ⇒ 语义完成、byte 层封顶（§6.2）。
//   [b28d-solo 复测] 474=474 vs 476、p167 CLEAR 20/20；P221/P227 临时链试形对 trace 槽
//   （ORIG -0xe8 深 vs 4.4 -0x28）**零效**（4.1 不复用 13 个 trace 槽，N82 真因）⇒ 封顶。
// [b29-Lane21 复核] 「trace 槽零效」系 fndiff 对齐错觉：两侧 lea -0xe8..-0x28 分布实测全等。
//   真修法三条已落：①`bool ok;` 前置声明（P212，ok=-0x15/db=-0x14 就位）；②`int col = 0;`
//   移到 fetch 检查之后（ORIG +0x1d1 col/i 连发）；③13 处具名 cMyTrace 改全表达式临时链
//   （P227：ctor 前 vararg 装 %ebx 跨 ctor 保活）。读数 ndiff 516→14（orig 474/our 476）。
// [b29-Lane21] 残差 11 行 = N1 常量拆分：ORIG `shl $2; add $0xcf10; add sig; lea 0x13`
//   vs 4.4 `add $0x33c8; shl $2; add sig; lea 3`（0xcf23 分解 mod16+0x13 vs mod4+3，同址
//   sig+0xcf23）；槽集两侧逐值全等 ⇒ 判据⑪ (a) 不成立，语义等价。
//   残差（N1 常量拆分，5 形实测）：ORIG `shl $2; add $0xcf10; add sig; lea 0x13` vs 4.4
//   `add $0x33c8; shl $2; add sig; lea 3`（0xcf23 非 4 对齐的拆分点不同）；&arr[K] /
//   arr+K / 视图指针 / 两段常量 / 行视图五形均折 add。
#include "DB_LoadEtc/LoadFairPvPScore.inc"
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc16LoadFairPvPScore…E9handleIdx
    // ORIG 槽序：ok(-0x15) 深于 db(-0x14) ⇒ 原码 ok 先声明（P212 全头部声明「先声明深」）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect private_win,private_lose,private_draw,relay_battle_win,"
                  " relay_battle_lose, relay_battle_draw,relay_battle_2kill,"
                  " relay_battle_3kill, successive_win, max_successive_win,"
                  " job_score,daily_play_count, unix_timestamp(last_play_time),"
                  " give_item from fair_pvp_score where charac_no=%u",
                  sig->m_characNo);
    // ORIG local_19（-0x15）为复用的 bool 结果槽：每步 call 后 `mov %al,-0x15;`
    // `movzbl; xor$1; test; je` —— 命名 bool ok 赋值再判触发同形。
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1618, 5)(
            "DB_LoadSkill::LoadFairPvPScore, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into fair_pvp_score (charac_no) values(%u)",
                      sig->m_characNo);
        ok = db->exec(true);                                // ORIG local_19 复用同槽
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1625, 5)(
                "DB_LoadEtc::LoadFairPvPScore insert, exec() ERROR"
                  " charac_no=%u", sig->m_characNo);
            return false;
        }
        return true;
    }

    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x162f, 5)(
            "DB_LoadEtc::LoadFairPvPScore, fetch() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    int col = 0;                                            // 8904461 movl $0,-0x10
    // 前 6 列：i∈[0,1] 外层循环内 3 个显式 get（ORIG 8904474..8904636 无内层循环，
    // 字段地址 = sig + i*0xc + 0xcf23/27/2b；行号 0x1638/0x163f/0x1646）。
    for (int i = 0; i <= 1; ++i)
    {
        ok = db->get_uint(col++, sig->m_fairPvpScore[i * 3 + 0]);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1638, 5)(
                "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
        ok = db->get_uint(col++, sig->m_fairPvpScore[i * 3 + 1]);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x163f, 5)(
                "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
        ok = db->get_uint(col++, sig->m_fairPvpScore[i * 3 + 2]);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1646, 5)(
                "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
    }
    // 4 个独立 uint 列（8904649.. 平铺 je 前进/失败落 log 岛；行号 1667/6e/75/7c）
    ok = db->get_uint(col++, sig->m_fairPvpScore[6]);        // successive_win +0xcf3b
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1667, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpScore[7]);        // max_successive_win +0xcf3f
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x166e, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpScore[8]);        // job_score +0xcf43
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1675, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpScore[9]);        // daily_play_count +0xcf47
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x167c, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }

    // give_item 压缩列：fail 无 log（ORIG else 直落 return 0 岛 @89053xx）
    if (!get_compressed_blob_data(db, col++, sig->m_fairPvpGiveItem, 0x650, 0))
    {
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpLastPlayTime);    // +0xd59b
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1689, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    // get_int 直写 sig+0xd59f（ORIG 无中间局部；行号 0x1690）
    ok = db->get_int(col++, sig->m_fairPvpLastPlaySec);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1690, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    if (sig->m_fairPvpLastPlaySec < getDailyInitializeTime(6, 0, 0))
    {
        sig->m_fairPvpLastPlayTime = 0;                  // ORIG movl $0x0,+0xd59b
    }
    // 末列 get_ubyte 直写 +0xd5a3（行号 0x16a5）
    ok = db->get_ubyte(col++, sig->m_fairPvpGiveFlag);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x16a5, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadBoosterGage @0x08446d9e —— member_booster_gage gage 单列
// 入 +0xeccc（accId 维度）。错行号 0xafc4。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x08446d9e。
// 已修（本波）：①`ok = db->get_int(...)` 复用同一 bool 槽再取反（P102；原写
//   `if (!db->get_int(...))` 直测 call 结果 ⇒ 少 mov %al,槽 + movzbl 两条）；
//   ②`bool ok;` 声明前置于 db 指针局部 ⇒ 槽序对齐 ORIG（ok=-0x15 深于 db=-0x14；
//   P212 全头部声明「先声明深」——声明序反了 4.4 给 db=-0x18/ok=-0x11）。
// 已修（前波）：nRows 具名 + 独立第二守卫（P133）+ col 取后自增 + trace 临时链。
#include "DB_LoadEtc/LoadBoosterGage.inc"
{
    static const int handleIdx = 2;    // ORIG _ZZN10DB_LoadEtc15LoadBoosterGage…E9handleIdx
    // ORIG 槽序：ok(-0x15) 深于 db(-0x14) ⇒ 原码 ok 先声明（P212 全头部声明「先声明深」）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect gage from member_booster_gage where m_id=%u",
                  sig->m_accId);
    ok = db->exec(true);                                     // local_19 保存复用
    if (!ok)
    {
        return false;
    }
    int nRows = db->get_n_rows();                            // ORIG -0x10 具名槽 + cmpl mem
    if (nRows != 1)
    {
        return false;
    }
    if (!ok)                                                 // ORIG 0x08446e31 独立第二守卫
    {
        return false;
    }
    int col = 0;                                             // ORIG -0xc 列号槽（取后自增）
    if (!db->fetch())
    {
        return false;
    }
    // ORIG 0x08446e7a：get_int 结果存回同一 ok 槽（-0x15）再取反（P102 复用）。
    ok = db->get_int(col++, sig->m_boosterGage);             // +0xeccc
    if (!ok)
    {
        // ORIG 0x08446e93：ctor 前 m_accId 装 %ebx 跨 ctor 保活（P227 临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0xafc4, 5)(
            "DB_LoadEtc::LoadBoosterGage, get(%d) ERROR m_id=%u", col,
            sig->m_accId);           // P226 语句表达式实测无增益（20→20），保留 P227 形
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadPcRoomPlayTimeInfo @0x084066ae —— 事件关闭时如实 ret 1；
// play_time(+0xc9b3)/reward_cnt(+0xc9b7)。无记录 ret 1。
// ============================================================================
// [b28d-solo-DB_LoadEtc] IDENTICAL/AE：ORIG 0x084066ae。
// 已修（本波，N92 突破口 = P197① union 双视角）：playTime 局部改
//   `union { unsigned int u; int i; }`——get_uint 出参走 u（unsigned int&），store 走
//   与字段 m_totalPcRoomPlayTime(int) 同型的 i ⇒ 复现 ORIG 单段
//   `mov -0x10(%ebp),%edx; mov 0xc(%ebp),%eax; mov %edx,0xc9b3(%eax)`；
//   直接 `unsigned 局部 → int 字段` 恒多 `mov %eax,%edx` 中转（N92，探针实证），
//   字段侧改 unsigned 会使 CUser::GetEtcInfo 0x08677ae1 直存形多中转（探针 h7）⇒ 不改共享头。
// 已修（前波）：GetRepeatEvent 链式去 ev 局部、NumberToString 内联实参（P259）。
#include "DB_LoadEtc/LoadPcRoomPlayTimeInfo.inc"
{
    // ORIG 0x084066b4：GetRepeatEvent 结果直留 %eax 调虚槽（无具名局部，P0 槽律）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x5c)->IsEventing((CUser*)0))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    // ORIG 0x0840670e：NumberToString 结果内联直进 set_query 实参（无 accStr 槽）。
    db->set_query("seLect play_time, reward_cnt from event_1204_pcroom_play_time"
                  " where m_id = %s and occ_date > curdate()",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    // ORIG 0x0840675e：两局部在 exec 检查之后、nRows 检查之前初始化。
    // N92 突破口（P197① union 双视角）：get_uint 需 unsigned int 左值，而字段
    //   m_totalPcRoomPlayTime 是 int——异型槽值 store 恒多 `mov %eax,%edx` 中转；
    //   union 让出参走 u、store 走同型 i，复现 ORIG 单段 `mov -0x10(%ebp),%edx`。
    union PlayTimeSlot
    {
        unsigned int u;
        int i;
    };
    PlayTimeSlot playTime;
    playTime.u = 0;
    unsigned char rewardCnt = 0;
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }
    if (!db->get_uint(0, playTime.u))
    {
        return false;
    }
    if (!db->get_ubyte(1, rewardCnt))
    {
        return false;
    }
    sig->m_totalPcRoomPlayTime = playTime.i;                 // +0xc9b3
    sig->m_pcRoomPlayTimeRewardCnt = rewardCnt;              // +0xc9b7
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadUsedGiftFatigueInfo @0x08406810 —— CUsedFatigueGiveItem 事件
// 驱动的 fatigue_quantity 装载（账号表 idx9 / 角色表 idx2 双路径）。
// ============================================================================
// [b29-Lane21] 已修：两守卫改 `||` 短路 + 取反（复现 ORIG `xor $1,%eax`）+ 早返；`quantity`
//   声明上移至 exec 之后（ORIG 0x084069c4 `movl $0x0,-0x20`）。残差 14 行（N 封顶）：
//   N7 callee-saved 角色（ORIG %ebx / 4.4 %esi 承载 NumberToString 保活 + 返回值物化）+
//   P265 单段装载（`mov -0x20(%ebp),%edx` vs 4.4 两段）+ EH 尾归一 3 条。
// b28d-31: N 封顶（P167 未闭合）——ORIG 0x08406810；p167 MISSING
// string 拷贝构造/dtor ×2 + toTString(const string&) ×1 + _Unwind_Resume
//（ORIG 具名 string 临时生命周期 + 重载选择差），语义等价但调用点未归零。
// [b28d-solo-DB_LoadEtc] N封顶（N18a/N7）：ORIG 0x08406810；166 vs 168、p167 CLEAR 20/20。
// 已修（本波）：①两组守卫改**正极性包裹** `if (evMng != 0 && evMng->is_eventing(0x65)) {…}
//   return true;`（P214①）⇒ 复现 ORIG 的短路物化（`cmpl $0,-0x14; je` + `test;je` +
//   `mov $1/$0; test; je`）并把槽组翻正（mng=-0x14/entity=-0x10/isCharac=-0x15/db=-0xc
//   与 ORIG 全等；旧独立 if-return 使槽整体深 4）；②NumberToString 改内联实参（P259，
//   值留 %ebx 跨 toTString call）。
// 残差（不可翻）：①ORIG 取反段 `xor $1,%eax` 属负极性短路物化，正极性包裹形恒缺（4.4 直接
//   `test %al,%al; je`）；②`mov $1,%ebx; jmp` 尾岛与 get_n_rows()==0 的 sete 物化属
//   4.1 共享出口/短路物化，4.4 恒折；③quantity store 的 eax→edx 中转（N18a）。
// 已修（前波）：具名 std::string tableName（p167 归零）、分支极性翻转（!isCharacEvent 直落 idx9/m_id）。
// [b28d-SweepDBLoadEtc2 本波] **真 bug（返回值门禁反向）**：原 `if (get_n_rows()==0 ||
//   !db->fetch()) return true;` 把 fetch 失败当成功返回；ORIG 0x084069c4..0x084069f6
//   为 n_rows==0 → 返回 1（`sete %al` 物化，P225）、fetch 失败 → 返回 0 ⇒ 拆两条独立
//   if。读数：gap 3→3、sete 段两侧一致（助记符直方图差仅返回值归一 esi/ebx 形）。
#include "DB_LoadEtc/LoadUsedGiftFatigueInfo.inc"
{
    sig->m_curCharacUsedGiftFatigueQuantity = 0;             // +0xc9b8 清零先

    EventClassify::CEventScriptMng* evMng =
        reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    // [b29-Lane21] ORIG 0x08406835 `cmpl $0,-0x14; je L1($1)` + 0x0840684e `xor $1;test;je
    //   L0($0)` + `test;je 主流程` ⇒ 原码是「`||` 短路 + 取反 + 无具名 bool」(P0：ORIG
    //   槽集无该 bool)；旧 P214① 正极性包裹形产 `test %al` 无 `xor` ⇒ DIFF。
    if (evMng == 0 || !evMng->is_eventing(0x65))
    {
        return true;
    }
    {
        EventClassify::CUsedFatigueGiveItem* entity =
            static_cast<EventClassify::CUsedFatigueGiveItem*>(
                evMng->get_event_entity(0x65));
        // ORIG 0x08406885：`movb $0,-0x15` 在 entity 存槽后、判空之前（声明点）。
        bool isCharacEvent = false;
        // 同第一段（ORIG 0x084068a1 `xor $1`）：`||` 短路 + 取反 + 早返。
        if (entity == 0 || !entity->getIsCharacEvent(isCharacEvent))
        {
            return true;
        }
        {
            MySQL* db = 0;
            // ORIG 0x084068d1：具名 std::string（_ZNSsC1ERKSs 到 -0x1c）+ 每分支各调 toTString
            // （p167 MISSING 指认 C1ERKSs/D1/toTString/_Unwind_Resume）。
            std::string tableName = entity->get_db_table_name();
            // ORIG 0x084068ea：`xor $1; je` 落 idx2 分支 ⇒ fall-through 为「非角色事件」idx9/m_id
            // 分支（P220 极性翻转；旧块序相反）。
            if (!isCharacEvent)
            {
                db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                       (ENUM_SERVER_GROUP)0);
                // ORIG 0x0840692a：NumberToString 结果留 %ebx 跨 toTString call 保活（P259 内联实参）。
                db->set_query("seLect fatigue_quantity from %s where m_id = %s",
                              toTString(tableName),
                              NumberToString(sig->m_accId, 0));
            }
            else
            {
                db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                       (ENUM_SERVER_GROUP)0);
                db->set_query("seLect fatigue_quantity from %s where charac_no = %u",
                              toTString(tableName), sig->m_characNo);
            }

            if (!db->exec(true))
            {
                return false;
            }
            // [b29-Lane21] ORIG 0x084069c4 `movl $0x0,-0x20(%ebp)` 紧贴 exec 之后
            // ⇒ quantity 声明在 get_n_rows 之前（先声明→更深槽）。
            unsigned int quantity = 0;
            // ORIG 0x084069c4..0x084069f6 真值门禁：get_n_rows()==0 → 返回 1
            // （`sete %al` 物化，P225）；fetch() 失败 → 返回 0。旧合并短路写法
            // `|| !db->fetch()` 返回 true 属真语义 bug（fetch 失败被当成功）。
            if (db->get_n_rows() == 0)
            {
                return true;
            }
            if (!db->fetch())
            {
                return false;
            }
            if (!db->get_uint(0, quantity))
            {
                return false;
            }
            sig->m_curCharacUsedGiftFatigueQuantity = quantity;
        }
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadAttendanceInfo @0x08448944 —— SIG_ATTENDANCE_EVENT 是当日
// 是否可签到的 out 参数（+0x00 flag，blob 其余不触）。事件关闭返 0。
// ============================================================================
#include "DB_LoadEtc/LoadAttendanceInfo.inc"
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
    // ORIG 8904a：NumberToString 结果直进实参（无保存槽）。
    db->set_query("seLect occ_date from event_1207_attendance where m_id = %s and"
                  " occ_date > from_unixtime(%u)", NumberToString(accId, 0),
                  resetBase);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();   // ORIG -0xc 保存+cmpl $0（后声明→浅槽）
    if (nRows == 0)
    {
        sig->m_isEnabled = 1;        // 当日尚可签到
    }
    return true;
}

// ============================================================================
// DB_LoadEtc::LoadSeriaRoomAniDeco @0x0844a20a —— seria 房间动画装饰双 short
// 至 +0x14b5c/+0x14b5e（combo 区尾部残段，超 SIG_COMBO_SKILL 0xb2 的部分）。
// ============================================================================
#include "DB_LoadEtc/LoadSeriaRoomAniDeco.inc"
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing((CUser*)0))
    {
        return false;
    }
    // ORIG：ok=-0xd（深）先声明、db=-0xc（浅）后声明（4.4 后声明→浅）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect receive_flow, deco_flow from event_1208_seriaroom_anideco"
                  " where m_id = %d and server_id = %d and charac_no = %d",
                  sig->m_accId, sig->m_serverGroup, sig->m_characNo);
    // ORIG 0844a2b0：exec 结果存 -0xd（深于 db=-0xc）→ ok 先声明。
    ok = db->exec(true);
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xba8f, "LoadSeriaRoomAniDeco Query Error : m_id(%d)",
                              sig->m_accId);
        return false;
    }
    // ORIG 0844a308-30f：test/setne 物化正条件（nRows != 0 才进 fetch 段），
    // 尾统一 return true。
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
        {
            return false;
        }
        if (!db->get_short(0,
                           *reinterpret_cast<short*>(&sig->m_comboSkill[0xb2])))  // +0x14b5c
        {
            return false;
        }
        if (!db->get_short(1,
                           *reinterpret_cast<short*>(&sig->m_comboSkill[0xb4])))  // +0x14b5e
        {
            return false;
        }
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
#include "DB_LoadEtc/LoadEventServerMessageInfo.inc"
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

    int nRows = db->get_n_rows();                            // local_18
    bool ok = false;                                         // local_11 复用结果槽
    char kind = 0;                                           // local_1d
    char msgIndex = 0;                                       // local_1e
    // ORIG 89045735.. 两段展开初始化：message[0x32] 走 2B头+rep stos+2B/1B尾
    // （= 大数组 {0} 初始化形），name[0x14] 走 5 条 movl（小数组）。
    char message[50] = { 0 };                                // local_6a
    char name[20] = { 0 };                                   // local_32
    int updateTime = 0;                                      // local_38
    int written = 0;                                         // -0x10（entry 索引）

    for (int i = 0; i < nRows; ++i)                          // -0xc（循环计数）
    {
        ok = db->fetch();
        if (!ok)
        {
            return false;
        }
        ok = db->get_byte(0, kind);                          // col0 kind
        if (!ok)
        {
            return false;
        }
        ok = db->get_byte(1, msgIndex);                      // col1 message_index
        if (!ok)
        {
            return false;
        }
        ok = db->get_str(2, name, 0x14);                     // col2 charac_name
        if (!ok)
        {
            return false;
        }
        ok = db->get_str(3, message, 0x32);                  // col3 message
        if (!ok)
        {
            return false;
        }
        ok = db->get_int(4, updateTime);                     // col4 update_time
        if (!ok)
        {
            return false;
        }

        // ORIG 89058e6..9cb：每个写点独立重算 sig + written*0x58 + base（P157 逐字段
        // operator[] 重读，不缓存 entry 指针）；strlen 结果直存 + strncpy 限长拷贝。
        sig->m_eventMessages[written].m_kind = (unsigned char)kind;       // +0x00
        sig->m_eventMessages[written].m_index = (unsigned char)msgIndex;  // +0x01
        sig->m_eventMessages[written].m_messageLen = strlen(message);     // +0x04
        // ORIG +0x2bf/+0x30f 用 strncpy(dst,src,strlen(src))，且 n 独立重调
        // strlen（P48 去缓存，不复用 m_messageLen）。
        strncpy(sig->m_eventMessages[written].m_message, message,
                strlen(message));                                        // +0x08
        sig->m_eventMessages[written].m_nameLen = strlen(name);           // +0x3c
        strncpy(sig->m_eventMessages[written].m_name, name,
                strlen(name));                                            // +0x40
        sig->m_eventMessages[written].m_updateTime = updateTime;          // +0x54
        ++written;
    }

    sig->m_eventMessageCount = (unsigned char)nRows;     // +0x2d38 movb 截断
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
    // ORIG 0x0844a38b 精确形：kind==0 早返 "combo_skill"（test/jne+mov+jmp 岛）、
    // ==1 → "combo_skill_2nd"、default "combo_skill"。
    if (kind == 0)
        return "combo_skill";
    if (kind == 1)
        return "combo_skill_2nd";
    return "combo_skill";
}

// ---- OnlyOrigBatch3：CHARAC_QUEST_SHOP_DATA（ORIG C1 W 0x0848f862 / clear W 0x0848f83e）----
#include "DB_LoadEtc/CHARAC_QUEST_SHOP_DATA.inc"
{
    clear();
}

#include "DB_LoadEtc/clear.inc"
{
    memset(this, 0, sizeof(CHARAC_QUEST_SHOP_DATA));   // ORIG memset(this,0,0xaa)
}
