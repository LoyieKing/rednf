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
// C1/C2 — DB_CreateCharac.md @0x08400cc2：基类 ctor + 设 vptr。
// D1/D2/D0 — ~.md @0x08400cde/@0x08400d0e：设 vptr + 基类析构；D0 另 delete。
// 非 inline 定义发射全套生命周期符号。
// ===========================================================================
DB_CreateCharac::DB_CreateCharac() : DBDispatcher()
{
}

DB_CreateCharac::~DB_CreateCharac()
{
}

// ===========================================================================
// dispatch — dispatch.md T @0x08400d2c
// ===========================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x08400d2c orig/ours=210/181 ndiff=180。
// 帧/槽平移 + Stream 读取链内联差；语义按报告逐条对齐。
bool DB_CreateCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    SIG_CREATE_CHARAC* sig = stream->GetOutBuffer<SIG_CREATE_CHARAC>();
    if (sig == NULL)
    {
        return false;
    }

    sig->m_resultCode = 0;

    bool ok = CheckLimitCreateNewCharac(sig, true);         // call 08401922
    if (ok)
    {
        if (CreateNewCharac(sig))                           // call 08401dcc
        {
            if (GetCharacNo(sig))                           // call 0840275c
            {
                if (_CreateNewCharacStat(sig))              // call 0840215e
                {
                    bool invOk;                             // -0xd(%ebp)
                    if (sig->m_giftFindOk != 0)             // byte +0x533f
                    {
                        invOk = CreateInventory(sig->m_characNo,
                                                reinterpret_cast<Inven_Item*>(sig->m_pad3a9));
                    }
                    else
                    {
                        invOk = CreateInventory(sig->m_characNo, NULL);
                    }
                    if (sig->m_equipFindOk != 0)            // byte +0x5340
                    {
                        // ORIG lea 0x4e01(%eax)：&m_giftItem 起点，压缩长度 0x2dc。
                        invOk = CreateEquip(sig->m_characNo, &sig->m_giftItem);
                    }
                    if (!invOk)
                    {
                        sig->m_resultCode = 2;
                    }
                    if (!CreateSkill(sig))
                    {
                        sig->m_resultCode = 2;
                    }
                    if (!CreatePvP(sig->m_characNo))
                    {
                        sig->m_resultCode = 2;
                    }
                    if (!_createCharacDungeon(sig->m_accId))
                    {
                        sig->m_resultCode = 2;
                    }
                    else if (_createCharacQuest(sig->m_characNo))
                    {
                        if (!_createCharacNPCRelationship(sig->m_characNo))
                        {
                            sig->m_resultCode = 2;
                        }
                        InsertLimitCreateNewCharac(sig, true);
                    }
                    else
                    {
                        sig->m_resultCode = 2;
                    }
                }
                else
                {
                    sig->m_resultCode = 2;
                }
            }
            else
            {
                sig->m_resultCode = 2;
            }
        }
    }

    return SendResult(userIdx, slot, sig) != 0;
}

// ===========================================================================
// _createCharacNPCRelationship — 报告 @0x0840103e
// seLect/inSert charac_npc 一条；尾部 exec 返回值即调用方判定值。
// ===========================================================================
bool DB_CreateCharac::_createCharacNPCRelationship(unsigned int characNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into charac_npc(charac_no,npc_cnt,npc_data) values(%d,0,'')",
        characNo);
    return db->exec(true);
}

// ===========================================================================
// _createCharacQuest — 报告 @0x08401094
// ===========================================================================
bool DB_CreateCharac::_createCharacQuest(unsigned int characNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("inSert into new_charac_quest(charac_no, clear_quest) values(%u,'')",
                  characNo);
    // P168a①：callee 返回 bool 一律裸形——`if (!db->exec(true)) return false;`
    // 直产 ORIG 尾岛 call;xor$1;test;je（N96 全域禁 register 绑定，旧形删除）。
    if (!db->exec(true))
        return false;
    return true;
}

// ===========================================================================
// _createCharacDungeon — 报告 @0x084010fc：无行则补插空 member_dungeon 行。
// ===========================================================================
bool DB_CreateCharac::_createCharacDungeon(unsigned int accId)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);

    db->set_query("seLect * from member_dungeon where m_id=%s",
                  NumberToString(accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into member_dungeon (m_id,dungeon) values (%s,'')",
                      NumberToString(accId, 0));
    }
    // P168a① 裸形（N96 清理）。
    if (!db->exec(true))
        return false;
    return true;
}

// ===========================================================================
// CheckLimitCreateNewCharac(SIG*) — 报告 @0x08401504
// 当日（本地零点起）已建 >1 角色拒建（rc=5）；exec 失败 rc=2。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08401504 orig/ours=109/108 ndiff=49。
// 残差为槽号 -0x18/-0x20 互换 + bool 物化形（movb/movzbl/xor vs 直 xor）；
// P112/N82 跨版本槽分配，语义等价封顶。
bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
{
    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    localtime_r(&nowSec, &localTime);
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;
    time_t dayStart = mktime(&localTime);

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
        if (!db->fetch())
        {
            break;
        }
        if (!db->get_uint(0, createTime))
        {
            break;
        }
        if ((time_t)createTime >= dayStart && ++todayCount > 1)
        {
            sig->m_resultCode = 5;
            return false;
        }
    }
    return true;
}

// ===========================================================================
// CheckLimitCreateNewCharac(SIG*, bool) — 报告 @0x08401922
// count>1 拒建并 trace [LIMIT NEW CHARAC MID]；bool 参不影响路径（ORIG 仅
// 存栈未读）。
// [b21 N·P102/P197-④] 语义已闭合：isGM 早返、GetDBHandle(1,0)+time+localtime_r 清
// 时、set_query(不查返)+exec、nRows==0 返 true、遍历 fetch/get_uint 双列、
// count>1→m_resultCode=5 + [LIMIT NEW CHARAC MID] trace(fmt,NumberToString,
// m_characNo,lastAccessTime,get_server_group) 拒建，调用链/常量/字段偏移逐条对齐。
// 残余：ORIG 把 fetch/get_uint 返回 materialize 到 `-0x29`（local_2d char 槽）再
// `movzbl;xor;test`（P102 同槽多查），4.4 对 `!a()||!b()||!c() break` 短路直跳；trace
// 实参寄存器缓存序 N82。无子母，封 N。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08401922 orig/ours=155/148 ndiff=104。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
                                                bool traceFlag)
{
    (void)traceFlag;

    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    localtime_r(&nowSec, &localTime);
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;
    nowSec = mktime(&localTime);   // ORIG mktime 结果写回 -0x20 同槽

    unsigned int count = 0;
    unsigned int lastAccessTime = 0;
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
    for (int i = 0; i < nRows; ++i)
    {
        if (!db->fetch() || !db->get_uint(0, count) ||
            !db->get_uint(1, lastAccessTime))
        {
            break;
        }
        if (count > 1)
        {
            sig->m_resultCode = 5;
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xf25, 0);
            trace("[LIMIT NEW CHARAC MID] %s,%d,%d,%d",
                  NumberToString(sig->m_accId, 0),
                  sig->m_characNo, lastAccessTime,
                  G_CEnvironment()->get_server_group());
            return false;
        }
    }
    return true;
}

// ===========================================================================
// makeRequestResetLimitCreateMid — 报告 @0x08401864
// channel_no==1 时经内部包请求重置中端限建表（==1 布局，匹配 ORIG sete 形式）。
// ===========================================================================
void DB_CreateCharac::makeRequestResetLimitCreateMid()
{
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xee3),
            true);
        ARAD::DISPATCHER::make_internal_stream_jpn(guard,
                                                   ARAD::ENUM_INTERNALPACKET_JPN(0x37),
                                                   -1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ===========================================================================
// _checkCharacName — 报告 @0x084011d4
// 重名/delete_flag 判定；gc_no==1010(0x3f2) 且恰与基准名相同时追查 query 日志。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x084011d4 orig/ours=120/117 ndiff=88。
// 帧/槽号与寄存器分配差（P112/N82）；语义按报告逐条对齐。
bool DB_CreateCharac::_checkCharacName(MySQL* db, char const* dbTable,
                                       SIG_CREATE_CHARAC* sig)
{
    db->set_query("seLect delete_flag from %s.charac_info where charac_name='%s'",
                  dbTable, sig->m_name);
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }

    int deleted = 0;
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    sig->m_resultCode = 2;
    // [b23S3C] P48 去缓存：无 nRows/badFetch 具名槽（ORIG rows 直判 + log 点
    // 二次 call；短路双 xor 直判无 al 落槽）；deleted!=0 反形（ORIG `test;je
    // gc块`、rc3 作 body，P191）；rowCnt/qs 在 ctor 前求值（ORIG +0x10c/
    // +0x17d call 位于 ctor 之前），值跨 ctor 4.1 落 %ebx/%esi 寄存器缓存
    // （4.4 槽=1 条差，N18a）。
    if (!db->fetch() || !db->get_int(0, deleted))
        return false;
    if (deleted != 0)
    {
        sig->m_resultCode = 3;
    }
    else
    {
        if (G_CEnvironment()->get_gc_no() == 0x3f2)
        {
            int rowCnt = db->get_n_rows();
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xd6a, 5);
            trace("already charac_name(%s) row(%d)", sig->m_name, rowCnt);
            // DAT_08c42c04 = "사이토"（ORIG 字符串常量）
            static const char BASELINE_NAME[] = "\xec\x82\xac\xec\x9d\xb4\xed\x86\xa0";
            if (strcmp(sig->m_name, BASELINE_NAME) == 0)
            {
                const char* qs = db->GetQueryString();
                cMyTrace queryTrace(
                    __PRETTY_FUNCTION__,
                    0xd6d, 5);
                queryTrace("already charac_name query(%s)", qs);
            }
        }
        sig->m_resultCode = 1;
    }
    return false;
}

// ===========================================================================
// _checkCreateEventCharacter — 报告 @0x084013b2
// 硬编码 2009-11-20..2010-01-03(+1s) 事件窗；窗内要求非事件角色标志为 0。
// [b22判词·N族块布局] 31/31 助记符集与语义逐条对齐：窗口双 getCurSec 右值内联
// （无 nowSec/inWindow 槽，P0/P183）、flag 判定 0x24、rc4=4@0x5344 两条独立
// return-false 路径。残余差异仅 return false/return true 三尾块的物理排列序
// （ORIG: flag!=0-rc4 → 窗外-rc4 → flag==0-ret1；我们: flag!=0-rc4 → ret1 →
// 窗外-rc4），系编译器块布局 N 族，同指令数、语义等价，结构不可复现。
// ===========================================================================
bool DB_CreateCharac::_checkCreateEventCharacter(SIG_CREATE_CHARAC* sig)
{
    // P0/P183：窗口计算右值内联（ORIG 寄存器持有，无 nowSec/inWindow 槽）；
    // getCurSec 每边独立调用（ORIG 两次 call）。
    if (GlobalData::s_systemTime_.getCurSec() > (int)0x4b055d6f &&
        GlobalData::s_systemTime_.getCurSec() <= (int)0x4b3bbf50)
    {
        if (sig->m_eventCharacFlag != 0)
        {
            sig->m_resultCode = 4;
            return false;
        }
    }
    else
    {
        sig->m_resultCode = 4;
        return false;
    }
    return true;
}

// ===========================================================================
// _updateCreateEventCharacter — 报告 @0x08401428
// 同事件窗内置 member_login.event_charac_flag=1 并回写角色标志。
// 52/52 逐条对齐：窗口右值内联（P0/P183）、GetDBHandle(6,0)+
// NumberToString(m_accId)+set_query+exec(true)、失败 rc4=2@0x5344、
// 成功 flag=1@0x24 返 1、窗外 rc4=4 块置函数尾（与 ORIG 同序）。
// 残余文本差均为 call/jmp 重定位与数据地址占位（§6.3），relink 后转 AE。
// ===========================================================================
bool DB_CreateCharac::_updateCreateEventCharacter(SIG_CREATE_CHARAC* sig)
{
    // P0/P183：窗口计算右值内联（ORIG 寄存器持有，无 nowSec/inWindow 槽）。
    if (GlobalData::s_systemTime_.getCurSec() > (int)0x4b055d6f &&
        GlobalData::s_systemTime_.getCurSec() <= (int)0x4b3bbf50)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                      E_SERVER_GROUP_NONE);
        db->set_query("upDate member_login set event_charac_flag = 1 where m_id = %s",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
        sig->m_eventCharacFlag = 1;
        return true;
    }
    sig->m_resultCode = 4;
    return false;
}

// ===========================================================================
// InsertLimitCreateNewCharac(SIG*) — 报告 @0x084016ae：新行插入 + trace。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x084016ae orig/ours=110/109 ndiff=38。
// 残差为 cMyTrace 具名对象槽号差（ORIG -0x34 vs 我们 -0x24）与寄存器保活差；
// P112/N82 跨版本槽分配，语义等价封顶。
bool DB_CreateCharac::InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
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

    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xe76, 0);
    trace("[LIMIT NEW CHARAC] %s,%d,%s,%d",
          NumberToString(sig->m_accId, 0),
          sig->m_characNo, dateBuf,
          G_CEnvironment()->get_server_group());
    return true;
}

// ===========================================================================
// InsertLimitCreateNewCharac(SIG*, bool) — 报告 @0x08401b9a
// count=count+1（无行插入 1），trace 行号 0xf61。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08401b9a orig/ours=141/140 ndiff=39。
// 同 trace 槽号/保活差（P112/N82）；语义等价封顶。
bool DB_CreateCharac::InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
                                                 bool traceFlag)
{
    (void)traceFlag;

    if (GlobalData::s_pGMAccounts_->isGM(sig->m_accId))
    {
        return true;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate limit_create_character set count=count+1, last_access_time=now() where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }
    if (db->getAffectedRowCount() == 0ULL)
    {
        db->set_query(
            "inSert into limit_create_character(m_id, count, last_access_time) values(%s, 1, now() )",
            NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
    }

    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTime;
    struct tm* lt = localtime_r(&nowSec, &localTime);
    char dateBuf[256];
    sprintf(dateBuf, "%04d-%02d-%02d %02d:%02d:%02d",
            lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
            lt->tm_hour, lt->tm_min, lt->tm_sec);

    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xf61, 0);
    trace("[LIMIT NEW CHARAC] %s,%d,%s,%d",
          NumberToString(sig->m_accId, 0),
          sig->m_characNo, dateBuf,
          G_CEnvironment()->get_server_group());
    return true;
}

// ===========================================================================
// CreateNewCharac — 报告 @0x08401dcc
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08401dcc orig/ours=234/230 ndiff=229。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
bool DB_CreateCharac::CreateNewCharac(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);

    db->set_query("seLect count(*) from charac_info where m_id=%s and delete_flag=0",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        sig->m_resultCode = 2;
        return false;
    }
    int liveCount = 0;
    if (!db->fetch() || !db->get_int(0, liveCount) || liveCount >= 0x24)
    {
        sig->m_resultCode = 2;
        return false;
    }

    // G_CEnvironment()->m_dbConnInfo[DB_HANDLE_2].m_db —— ORIG map::operator[]
    // 于 env+0x68、STDBConnInfo m_db 在 +0x42；经 get_db_conn_info() 访问
    //（CEnvironment.h OffsetCleanup2 添加的 inline accessor，不产生符号）。
    std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo>& connMap =
        G_CEnvironment()->get_db_conn_info();
    ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;       // 栈上局部初始值 2
    STDBConnInfo& connInfo = connMap[connIdx];
    if (!_checkCharacName(db, connInfo.m_db, sig))
    {
        return false;
    }

    sig->m_pad534d[0] = 0;                          // byte +0x534d

    // serverGroup==10 && now<0x478e7051 的临时通道：追加老号数量检查，
    // 若该账号从无角色则记 m_pad534d[0]=1。
    if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 10 &&
        GlobalData::s_systemTime_.getCurSec() < (unsigned int)0x478e7051)
    {
        db->set_query("seLect count(*) from charac_info where m_id=%s",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
        int totalCount = 0;
        if (!db->fetch() || !db->get_int(0, totalCount))
        {
            sig->m_resultCode = 2;
            return false;
        }
        if (totalCount == 0)
        {
            sig->m_pad534d[0] = 1;
        }
    }

    char escapedName[60];
    memset(escapedName, 0, sizeof(escapedName));
    db->escape_string(escapedName, sig->m_name);

    if ((signed char)sig->m_field23 >= 1)
    {
        if (!_checkCreateEventCharacter(sig))
        {
            return false;
        }
        _updateCreateEventCharacter(sig);
    }
    else if ((signed char)sig->m_field23 < 0)
    {
        sig->m_resultCode = 4;
        return false;
    }

    MakeInsertCharacInfo(db, sig, escapedName);
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x10e2, 5);
        trace("DB_CreateCharac::CreateNewCharac, exec() ERROR m_id=%s",
              NumberToString(sig->m_accId, 0));
        sig->m_resultCode = 1;
        return false;
    }
    return true;
}

// ===========================================================================
// _CreateNewCharacStat — 报告 @0x0840215e
// ===========================================================================
bool DB_CreateCharac::_CreateNewCharacStat(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into charac_stat (charac_no,HP, forbidden_to_play, forbidden_due_to) values(%d,100,0,0)",
        sig->m_characNo);
    if (!db->exec(true))
    {
        // N96 清理：register asm("ebx") 绑定删除。ORIG 为 4.1 ctor 前求值并
        // %ebx 跨调用保活（N80 三探针不可翻，P197② 语句表达式亦回退帧漂）；
        // 取合法裸内联形（NDIFF 最优），语义与调用点压参同值。
        cMyTrace(__PRETTY_FUNCTION__, 0x10fb, 5)(
            "DB_CreateCharac::_CreateNewCharacStat, exec() ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        sig->m_resultCode = 2;
        return false;
    }
    return true;
}

// ===========================================================================
// CreateInventory — 报告 @0x08402230
// item==NULL 仅写 coin；否则将 item 起点 0x3b55 字节 zip 后入库 + 空背包扩容表。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82/N78] ORIG 0x08402230 orig/ours=94/88 ndiff=90。
// 大缓冲（zipped 0x473e）清零/搬移形态差（N78）+ 帧槽（N82）。
bool DB_CreateCharac::CreateInventory(unsigned int characNo, Inven_Item* item)
{
    static const int handleIdx = 3;
    char zipped[0x473e];
    memset(zipped, 0, sizeof(zipped));
    unsigned int zippedLen = 0x473e;
    if (item != NULL &&
        !compress_zip(zipped, zippedLen, reinterpret_cast<char*>(item), 0x3b55))
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    if (item == NULL)
    {
        int coinOnCreate = WongWork::CEventMgr::GetCoinCharacCreate();
        db->set_query("inSert into inventory (charac_no, coin) values(%u, %d)",
                      characNo, coinOnCreate);
    }
    else
    {
        int coinOnCreate = WongWork::CEventMgr::GetCoinCharacCreate();
        db->set_query("inSert into inventory (charac_no, inventory,coin) values(%u, '%s',%d)",
                      characNo, db->blob_to_str(0, zipped, zippedLen),
                      coinOnCreate);
    }
    if (!db->exec(true))
    {
        return false;
    }

    db->set_query(
        "inSert into charac_inven_expand(charac_no,cargo,cargo_capacity,jewel) values(%u,'',0,'')",
        characNo);
    return db->exec(true);
}

// ===========================================================================
// CreateEquip — 报告 @0x084023ac
// item==NULL 直接成功；否则 zip(item, 0x2dc) upDate equipslot 列。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x084023ac orig/ours=88/53 ndiff=86。
// ORIG push %edi/%ebx + 大缓冲帧形态 vs 4.4 栈槽（P112/N82）；语义等价封顶。
bool DB_CreateCharac::CreateEquip(unsigned int characNo, Inven_Item* item)
{
    static const int handleIdx = 3;
    if (item == NULL)
    {
        return true;
    }

    char zipped[0x37a];
    memset(zipped, 0, sizeof(zipped));
    unsigned int zippedLen = 0x37a;
    if (!compress_zip(zipped, zippedLen, reinterpret_cast<char*>(item), 0x2dc))
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate inventory set equipslot='%s' where charac_no = %u",
                  db->blob_to_str(0, zipped, zippedLen), characNo);
    return db->exec(true);
}

// ===========================================================================
// CreateSkill — 报告 @0x084024e6
// m_masteredSkillA/B 各 0x198 zip 后入 skill 表两 slot blob。
// ===========================================================================
// b28d-D42 verdict=DIFF [N82/N78] ORIG 0x084024e6 orig/ours=148/87 ndiff=146。
// 大缓冲（bufSlot 0x1f5 区）rep stos/memcpy 形态差（N78）+ 帧槽（N82）。
bool DB_CreateCharac::CreateSkill(SIG_CREATE_CHARAC* sig)
{
    static const int handleIdx = 3;
    char bufSlot[0x1f5];           // 帧 -0x402 区（skillB 压缩输出，ORIG 501B）
    char bufSecond[0x1f5];         // 帧 -0x20d 区（skillA 压缩输出，ORIG 501B）
    memset(bufSlot, 0, sizeof(bufSlot));
    memset(bufSecond, 0, sizeof(bufSecond));
    unsigned int lenSlot = 0x1f5;     // -0x18
    unsigned int lenSecond = 0x1f5;   // -0x14

    // ORIG 第一次调用 @0x84025bd：dst=-0x20d、lenPtr=-0x14、src=sig+0x79(skillA)。
    if (!compress_zip(bufSecond, lenSecond,
                      reinterpret_cast<const char*>(sig->m_masteredSkillA), 0x198))
    {
        return false;
    }
    // ORIG 第二次调用 @0x84025f7：dst=-0x402、lenPtr=-0x18、src=sig+0x211(skillB)。
    if (!compress_zip(bufSlot, lenSlot,
                      reinterpret_cast<const char*>(sig->m_masteredSkillB), 0x198))
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    // blob 序：skill_slot ← blob(col=0,-0x20d,len-0x14)；skill_slot_2nd ← blob(col=1,-0x402,len-0x18)。
    db->set_query(
        "inSert into skill (charac_no, remain_sp ,skill_slot, skill_slot_2nd) values(%u,%u, '%s', '%s')",
        sig->m_characNo, sig->m_spAtLevelUp,
        db->blob_to_str(0, bufSecond, lenSecond),
        db->blob_to_str(1, bufSlot, lenSlot));
    return db->exec(true);
}

// ===========================================================================
// CreatePvP — 报告 @0x084026ec
// ===========================================================================
bool DB_CreateCharac::CreatePvP(unsigned int characNo)
{
    // N93 翻案（P212 标量例外）：4.1 先声明占浅槽（db@-0xc，ok@-0xd 后声明更深）；
    // 4.4.7 相反——把 ok 先声明（无初始化、仅赋值使用），db 后声明，
    // 4.4 槽序翻为 ok 深 db 浅 = ORIG 位。register asm("al") 绑定（N96 违例）删除，
    // 裸 `if(!ok)` 形与 ORIG mov%al,-off;movzbl;xor$1;test;je 同形（P168a①）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("inSert into pvp_result (charac_no) values(%u)", characNo);
    ok = db->exec(true);
    if (!ok)
        return false;
    return true;
}

// ===========================================================================
// GetCharacNo — 报告 @0x0840275c：@@identity → sig->m_characNo。
// ===========================================================================
bool DB_CreateCharac::GetCharacNo(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // P211 异型化翻法（探针 /tmp/bl10/p9.cpp w1/w2）：局部声明 **int** 而字段
    // unsigned ⇒ 4.4 store 翻为 ORIG 的 RHS 值先载（`mov -0xc,%edx; mov p,%eax`），
    // 无额外中转条数；同型 unsigned 恒 LHS 地址先载。
    int identity = (int)WongWork::DBCommon::GetIdentity(db);
    if (identity == 0)
    {
        return false;
    }
    sig->m_characNo = identity;
    return true;
}

// ===========================================================================
// SendResult — 报告 @0x084027b0
// 大流包：userIdx、slot、SIG 全量 0x5354 二进制 → DISPATCH_Q。
// ===========================================================================
int DB_CreateCharac::SendResult(int userIdx, int slot, SIG_CREATE_CHARAC* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x11db),
                       true);
    guard.operator*()->operator<<(userIdx);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);          // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x5354);  // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// 判例（ADeepY4[3]）：DIFF(N封)，gap +1（orig 154 / our 155）。已收：blob_to_str 调用序
// 归位（先 col=1 spec 后 col=0 elem，ORIG +0xaf/+0xd7）+ job 数据区改 pack(1) 具名视图
// 成员访问（reinterpret_cast<视图*>后 .成员），4.4-O0 收敛为直接位移载入 `mov off(%eax)`
// （pointer-cast 索引的 `add $off;mov (%eax)` 形全部消失），168→155。残余 = 1 条：ORIG
// 用 callee-saved %edi 承载 maxHP（+0x26，跨 NumberToString/set_query 实参布建存活，
// 故 push/pop %edi），我们落栈槽回读（N82 寄存器分配差）；实参布建 ORIG 用 %eax、我们用
// %edx（寄存器文本差，_NEAR_）。语义/callsite 完整。
// b28d-D42 verdict=DIFF [N82] ORIG 0x080ec2a0 orig/ours=154/155 ndiff=153。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
void DB_CreateCharac::MakeInsertCharacInfo(MySQL* db, SIG_CREATE_CHARAC* sig,
                                           char const* escapedName)
{
    int eventCharacLevel = (signed char)sig->m_field23;   // +0x23 列 event_charac_level
    int sex = (signed char)sig->m_ssnCheckFail;           // +0x534c ← ORIG %esi（sex 列位）
    int fatigueMax = DEFAULT_MAX_FATIGUE;

    MiciJobView const* jv = reinterpret_cast<MiciJobView const*>(sig);
    unsigned int expertJob   = static_cast<unsigned int>(jv->m_expertJob);      // +0x74
    int jumpValue            = static_cast<int>(jv->m_jump);                    // +0x72
    int hitRecovery          = static_cast<int>(jv->m_hitRecovery);             // +0x70
    unsigned int castSpeed   = static_cast<unsigned int>(jv->m_castSpeed);      // +0x6e
    unsigned int attackSpeed = static_cast<unsigned int>(jv->m_attackSpeed);    // +0x6c
    unsigned int moveSpeed   = static_cast<unsigned int>(jv->m_moveSpeed);      // +0x68
    int mpRegenShort         = static_cast<int>(jv->m_mpRegen);                 // +0x66
    int hpRegenShort         = static_cast<int>(jv->m_hpRegen);                 // +0x64
    unsigned int invenWeight = static_cast<unsigned int>(jv->m_invenWeight);    // +0x60
    // 第一次 blob_to_str(col=1, sig+0x3e, 0x22) → spec_property '%s'（ORIG 调用序在前）
    char const* specPropertyBlob =
        db->blob_to_str(1, const_cast<unsigned char*>(jv->m_specProperty), 0x22);
    // 第二次 blob_to_str(col=0, sig+0x36, 8) → element_resist '%s'
    char const* elementResistBlob =
        db->blob_to_str(0, const_cast<unsigned char*>(jv->m_elementResist), 8);
    unsigned int magDefense = static_cast<unsigned int>(jv->m_magDefense);      // +0x34
    unsigned int magAttack  = static_cast<unsigned int>(jv->m_magAttack);       // +0x32
    unsigned int phyDefense = static_cast<unsigned int>(jv->m_phyDefense);      // +0x30
    unsigned int phyAttack  = static_cast<unsigned int>(jv->m_phyAttack);       // +0x2e
    unsigned int maxMP      = static_cast<unsigned int>(jv->m_maxMp);           // +0x2a
    unsigned int maxHP      = static_cast<unsigned int>(jv->m_maxHp);           // +0x26

    db->set_query(
        "inSert into charac_info (m_id,charac_name,job,sex,maxHP,maxMP,phy_attack,"
        "phy_defense,mag_attack,mag_defense,element_resist,spec_property,"
        "inven_weight,hp_regen,mp_regen,move_speed,attack_speed,cast_speed,"
        "hit_recovery,jump,charac_weight,max_fatigue,create_time,expert_job,"
        "event_charac_level) values(%s,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',"
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW(),%d,%d)",
        NumberToString(sig->m_accId, 0), escapedName,
        (int)(signed char)sig->m_slot,     // job 列 ← sig+0x22（ORIG %ebx）
        sex,                               // sex 列 ← sig+0x534c（ORIG %esi）
        maxHP, maxMP, phyAttack, phyDefense, magAttack, magDefense,
        elementResistBlob, specPropertyBlob,
        invenWeight, hpRegenShort, mpRegenShort, moveSpeed, attackSpeed,
        castSpeed, hitRecovery, jumpValue, expertJob, fatigueMax,
        (int)(signed char)sig->m_pad25,    // expert_job 列 ← sig+0x25（ORIG edi）
        eventCharacLevel);                 // event_charac_level ← sig+0x23
}