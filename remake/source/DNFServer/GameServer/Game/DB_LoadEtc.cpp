#define _GLIBCXX_IOSTREAM 1
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
#include "DB_LoadEtc/get_compressed_blob_data.inc"
                              int recordSize);
#include "DB_LoadEtc/getDailyInitializeTime.inc"

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

#include "DB_LoadEtc/GetCurrentResetBaseTime.inc"

#include "DB_LoadEtc/DB_LoadEtc.inc"


#include "DB_LoadEtc/dtor_DB_LoadEtc.inc"


#include "DB_LoadEtc/GetUserRecord.inc"


#include "DB_LoadEtc/GetUserPvpBase.inc"


#include "DB_LoadEtc/GetUserPvp.inc"

#include "DB_LoadEtc/LoadLitmitItem.inc"


#include "DB_LoadEtc/dispatch.inc"


#include "DB_LoadEtc/LoadProperDungeonClearCount.inc"


#include "DB_LoadEtc/LoadConditionEventInfo.inc"


#include "DB_LoadEtc/_loadNPCRelasionship.inc"


// ============================================================================
// DB_LoadEtc::LoadRedeemItem @0x846a6a？—— 报告头地址 0x08406a6a；
// redeem_info blob → uncompress_zip → SIG_LOAD_ETC+0x40ce（m_expandData6 区）。
// ============================================================================

#include "DB_LoadEtc/LoadRedeemItem.inc"


#include "DB_LoadEtc/LoadPvPMissionList.inc"


#include "DB_LoadEtc/LoadAchievement.inc"


#include "DB_LoadEtc/LoadOnceGiveAvengerTitle.inc"


#include "DB_LoadEtc/LoadAuraAvatarOption.inc"


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


#include "DB_LoadEtc/SendResult.inc"


// ============================================================================
// DB_LoadEtc::_loadNPCRelasionship 见上；LoadQuestShop/_LoadQuestShop 下。
// ============================================================================

#include "DB_LoadEtc/_LoadQuestShop.inc"


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

#include "DB_LoadEtc/LoadTitleBook.inc"


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
                              0xbab4, "DB_LoadETC ERROR : LoadComboSkill Fail");
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
                              0xbaba, "DB_LoadETC ERROR : LoadComboSkill Second Fail");
        sig->m_field8 = 1;
    }
    }
}


#include "DB_LoadEtc/LoadComboSkill.inc"


#include "DB_LoadEtc/getComboSkill.inc"


#include "DB_LoadEtc/GetUserSkill.inc"


#include "DB_LoadEtc/LoadCharacItemStat.inc"


#include "DB_LoadEtc/LoadItemMakingSkill.inc"


#include "DB_LoadEtc/LoadTODUserState.inc"


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

#include "DB_LoadEtc/LoadAccountFirstLogin.inc"


#include "DB_LoadEtc/LoadAccountFirstLoginDaily.inc"


#include "DB_LoadEtc/LoadFairPvPScore.inc"


#include "DB_LoadEtc/LoadBoosterGage.inc"


#include "DB_LoadEtc/LoadPcRoomPlayTimeInfo.inc"


#include "DB_LoadEtc/LoadUsedGiftFatigueInfo.inc"


#include "DB_LoadEtc/LoadAttendanceInfo.inc"


#include "DB_LoadEtc/LoadSeriaRoomAniDeco.inc"


#include "DB_LoadEtc/LoadEventServerMessageInfo.inc"


// ============================================================================
// get_skill_table_name @0x083fbdc6：PvP 技能树频道或显式第二表 →
// "skill_fair_pvp"（ORIG 0x8c36689 raw，无 .skill 后缀），否则 "skill"
// （ORIG 0x8c36698 raw）。
// ============================================================================
char const* get_skill_table_name(bool use2nd)
{
    if (G_GameWorld()->IsPvPSkilTreeChannel() || use2nd)
        return "skill_fair_pvp";
    return "skill";
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

#include "DB_LoadEtc/CHARAC_QUEST_SHOP_DATA.inc"


#include "DB_LoadEtc/clear.inc"

