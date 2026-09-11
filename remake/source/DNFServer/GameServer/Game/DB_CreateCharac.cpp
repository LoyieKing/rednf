// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_CreateCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
extern int DEFAULT_MAX_FATIGUE;   // ORIG 全局（声明见 CUserCharacInfo.h:285）
// ============================================================================
// df_game_r 还原 —— DB_CreateCharac 实现（26 ORIG 符号）
// 逐符号转写自 docs/class_func_reports/DB_CreateCharac/（汇编+反编译 C 全文）。
// SQL 字符串、执行顺序、错误码（SIG_CREATE_CHARAC::m_resultCode @+0x5344）
// 与 ORIG 指令一一对应；未建模 blob 区以显式偏移取址（原始偏移已注释），
// 不造任何 SQL 结果假值。
// 唯一 owner：本 TU。ORIG 地址核对（nm --defined-only df_game_r）：
//   C1/C2 0x08400cc2 | D1 0x08400cde | D0 0x08400d0e | dispatch 0x08400d2c
//   _createCharacNPCRelationship 0x0840103e | _createCharacQuest 0x08401094
//   _createCharacDungeon 0x084010fc | _checkCharacName 0x084011d4
//   _checkCreateEventCharacter 0x084013b2 | _updateCreateEventCharacter 0x08401428
//   CheckLimit 0x08401504/0x08401922 | InsertLimit 0x084016ae/0x08401b9a
//   makeRequestResetLimitCreateMid 0x08401864 | CreateNewCharac 0x08401dcc
//   _CreateNewCharacStat 0x0840215e | CreateInventory 0x08402230
//   CreateEquip 0x084023ac | CreateSkill 0x084024e6 | CreatePvP 0x084026ec
//   GetCharacNo 0x0840275c | SendResult 0x084027b0 | MakeInsertCharacInfo 0x080ec2a0
// ============================================================================

#include "DB_CreateCharac.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "MySQL.h"
#include "DBMgr.h"
#include "CEventMgr.h"             // WongWork::CEventMgr::GetCoinCharacCreate
#include "GlobalData.h"             // s_db_mgr/s_pGMAccounts_/s_systemTime_/
                                    // s_stream_pool/s_big_stream_pool/s_msgq_mgr
#include "SigTypes.h"
#include "CGuildServerProxy.h"      // WongWork::CGMAccounts::isGM
#include "CEnvironment.h"           // G_CEnvironment/m_dbConnInfo/m_serverEnvir
#include "APSystemTypes.h"          // compress_zip
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "Arad_InternalDispatcher.h"
#include "GameTypes.h"              // cMyTrace

namespace WongWork { namespace DBCommon {
unsigned int GetIdentity(MySQL* db);   // ORIG T 0x083f9ad4（LinkClosureB_Misc.cpp 唯一 owner）
} }

char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b

// ===========================================================================
// MakeInsertCharacInfo — 报告 @0x080ec2a0
// charac_info 一行 INSERT。job 数据区（+0x26..+0x74）按 ORIG 位移寻址复现：
// 经 pack(1) 视图成员访问（reinterpret_cast<视图*> 后 .成员），使 4.4-O0 收敛为
// 直接位移载入 `mov off(%eax),%eax`（与 ORIG 逐条一致），避免 pointer-cast 索引
// 退化成 `add $off;mov (%eax)`。字节偏移 +0x26..+0x74 与 SIG_CREATE_CHARAC 对齐。
// ===========================================================================

#pragma pack(push, 1)
// MakeInsertCharacInfo job 数据视图：相对 sig 基址 +0x26..+0x74（pack 1，非对齐）。
struct MiciJobView
{
    unsigned char m_pre[0x26];      // sig+0x00..+0x25（本函数不用）
    unsigned int  m_maxHp;          // +0x26
    unsigned int  m_maxMp;          // +0x2a
    unsigned short m_phyAttack;     // +0x2e
    unsigned short m_phyDefense;    // +0x30
    unsigned short m_magAttack;     // +0x32
    unsigned short m_magDefense;    // +0x34
    unsigned char m_elementResist[8];   // +0x36
    unsigned char m_specProperty[0x22]; // +0x3e
    unsigned int  m_invenWeight;    // +0x60
    short m_hpRegen;                // +0x64
    short m_mpRegen;                // +0x66
    unsigned int  m_moveSpeed;      // +0x68
    unsigned short m_attackSpeed;   // +0x6c
    unsigned short m_castSpeed;     // +0x6e
    short m_hitRecovery;            // +0x70
    short m_jump;                   // +0x72
    unsigned int  m_expertJob;      // +0x74
};
#pragma pack(pop)

#include "DB_CreateCharac/MakeInsertCharacInfo.inc"


#include "DB_CreateCharac/DB_CreateCharac.inc"
{
}

#include "DB_CreateCharac/dtor_DB_CreateCharac.inc"
{
}

// ===========================================================================
// dispatch — dispatch.md T @0x08400d2c
// ===========================================================================
#pragma pack(push, 1)
// +0x3a9 起点视图：CreateInventory 的 Inven_Item* 源（ORIG `lea 0x3a9(%eax),%edx`；
// 数组名退化在 4.4 恒折 add+mov）。
struct SigGiftInvenView
{
    unsigned char m_pre[0x3a9];
    Inven_Item m_inven;                // +0x3a9
};
#pragma pack(pop)

#include "DB_CreateCharac/dispatch.inc"


#include "DB_CreateCharac/_createCharacNPCRelationship.inc"


#include "DB_CreateCharac/_createCharacQuest.inc"


#include "DB_CreateCharac/_createCharacDungeon.inc"


#include "DB_CreateCharac/CheckLimitCreateNewCharac.inc"
{
    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    // P102：fetch/get_uint 两次检查复用同一 bool 局部（ORIG 同一 -0x19 槽）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    localtime_r(&nowSec, &localTime);
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;
    // ORIG mktime 结果写回 -0x20 同槽（nowSec 复用，无独立 dayStart 槽）。
    nowSec = mktime(&localTime);

    unsigned int createTime = 0;
    db->set_query(
        "seLect unix_timestamp(create_time) from limit_create_character where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }

    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }
    int todayCount = 0;
    for (int i = 0; i < nRows; ++i)
    {
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        ok = db->get_uint(0, createTime);
        if (!ok)
        {
            break;
        }
        // P225 推广：拆嵌套 if 消 4.4 的 && bool 物化（ORIG 两跳同一目标）；
        // 比较写成 nowSec<=createTime（unsigned long 语义）复现 ORIG ja 无符号跳。
        if (nowSec <= createTime)
        {
            ++todayCount;
            if (todayCount > 1)
            {
                sig->m_resultCode = 5;
                return false;
            }
        }
    }
    return true;
}

#include "DB_CreateCharac/CheckLimitCreateNewCharac_1.inc"
                                                bool traceFlag)
{
    // ORIG 头部 `mov 0x10(%ebp),%eax; mov %al,-0x7c(%ebp)`：bool 形参规范化
    // spill（空 if 体被 DCE，仅留 spill 两条）。
    if (traceFlag)
    {
    }

    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    // P102：三次结果检查复用同一 bool 局部（ORIG 同一 -0x29 槽 + 三独立 break 岛）；
    // 声明在 db 之前使 4.4 给 bool 更深槽（-0x2c 尾 -0x29），db 落 -0x28。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    localtime_r(&nowSec, &localTime);
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;
    nowSec = mktime(&localTime);   // ORIG mktime 结果写回 -0x20 同槽

    // ORIG 初始化序 -0x44 先/-0x48 后 ⇒ lastAccessTime 先声明（4.4 后声明深槽）。
    unsigned int lastAccessTime = 0;
    unsigned int count = 0;
    db->set_query(
        "seLect count, unix_timestamp(last_access_time) from limit_create_character where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }

    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }
    // P232：ORIG 多一个 -0x20 死槽（`movl $0x0,-0x20` 后无读取）⇒ 补 dead 局部
    // 使后续标量槽（nRows/db/nowSec/trace）整体深 4 对齐。
    int deadSlot = 0;
    (void)deadSlot;
    for (int i = 0; i < nRows; ++i)
    {
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        ok = db->get_uint(0, count);
        if (!ok)
        {
            break;
        }
        ok = db->get_uint(1, lastAccessTime);
        if (!ok)
        {
            break;
        }
        if (count > 1)
        {
            sig->m_resultCode = 5;
            // P227 全表达式临时链：ORIG ctor 前右起求值并以 %esi/%edi/%ebx 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0xf25, 0)(
                "[LIMIT NEW CHARAC MID] %s,%d,%d,%d",
                NumberToString(sig->m_accId, 0),
                sig->m_characNo, lastAccessTime,
                G_CEnvironment()->get_server_group());
            return false;
        }
    }
    return true;
}

#include "DB_CreateCharac/makeRequestResetLimitCreateMid.inc"


#include "DB_CreateCharac/_checkCharacName.inc"


#include "DB_CreateCharac/_checkCreateEventCharacter.inc"


#include "DB_CreateCharac/_updateCreateEventCharacter.inc"


#include "DB_CreateCharac/InsertLimitCreateNewCharac.inc"
{
    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into limit_create_character(m_id, create_time) values(%s, now() )",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 5;
        return false;
    }

    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    struct tm* lt = localtime_r(&nowSec, &localTime);
    char dateBuf[256];
    sprintf(dateBuf, "%04d-%02d-%02d %02d:%02d:%02d",
            lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
            lt->tm_hour, lt->tm_min, lt->tm_sec);

    // P227 全表达式临时链：ORIG 在 ctor 前右起求值 varargs 并以 %esi/%edi/%ebx
    // 跨 ctor 保活（0840182b 段），具名对象形恒 ctor 后求值。
    cMyTrace(__PRETTY_FUNCTION__, 0xe76, 0)(
        "[LIMIT NEW CHARAC] %s,%d,%s,%d",
        NumberToString(sig->m_accId, 0),
        sig->m_characNo, dateBuf,
        G_CEnvironment()->get_server_group());
    return true;
}

#include "DB_CreateCharac/InsertLimitCreateNewCharac_1.inc"


#include "DB_CreateCharac/CreateNewCharac.inc"


#include "DB_CreateCharac/_CreateNewCharacStat.inc"


#include "DB_CreateCharac/CreateInventory.inc"


#include "DB_CreateCharac/CreateEquip.inc"


#include "DB_CreateCharac/CreateSkill.inc"


#include "DB_CreateCharac/CreatePvP.inc"


#include "DB_CreateCharac/GetCharacNo.inc"


#include "DB_CreateCharac/SendResult.inc"
