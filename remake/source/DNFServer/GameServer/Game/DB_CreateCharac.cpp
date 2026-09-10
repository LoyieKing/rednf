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

// b28d-solo-DB_CreateCharac verdict=DIFF(N7 封顶) ORIG 0x080ec2a0 orig/ours=154/153。
// 已收：①blob_to_str 调用序（先 col=1 spec 后 col=0 elem）②P183 去 jv 指针缓存（每个字段
// 重读 sig 形参 `mov 0x10(%ebp),%eax; mov off(%eax)`，与 ORIG 逐条一致）③maxHP 不设局部、
// 实参内联（%edi 承载，push/pop %edi 与帧 0xcc 归位）④+0x25 改具名局部 -0x60、+0x534c 内联
// （ORIG 前者有槽、后者 %esi 无槽）⑤帧/槽位/-0x64..-0x1c 全对齐、%edi/%esi/%ebx 角色一致。
// 残余 = 1 条：ORIG `call NumberToString; mov %eax,%edx`（结果移 %edx，压栈用 %eax 中转），
// 4.4 恒留 %eax、改用 %edx 中转压栈 ⇒ 助记符序列全等仅中转寄存器角色互换（N7 族；试过
// P226 裸语句表达式/具名局部/register 提示/cast 四形，均不翻）。语义/callsite 完整。
void DB_CreateCharac::MakeInsertCharacInfo(MySQL* db, SIG_CREATE_CHARAC* sig,
                                           char const* escapedName)
{
    int eventCharacLevel = (signed char)sig->m_field23;   // +0x23 列 event_charac_level
    // ORIG +0x25 值有具名槽 -0x60（P0：槽往返 ⇒ 有局部）；+0x534c 无槽（%esi 承载）⇒
    // 两者处理互换。
    int sexAt0x25 = (signed char)sig->m_pad25;            // +0x25 ← ORIG -0x60 槽
    int fatigueMax = DEFAULT_MAX_FATIGUE;

    // P183：ORIG 每个字段都重读 sig 形参（mov 0x10(%ebp),%eax; mov off(%eax),%eax）
    // ⇒ 禁指针缓存，逐点内联视图 cast。
    unsigned int expertJob   = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_expertJob);   // +0x74
    int jumpValue            = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_jump);                 // +0x72
    int hitRecovery          = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_hitRecovery);          // +0x70
    unsigned int castSpeed   = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_castSpeed);   // +0x6e
    unsigned int attackSpeed = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_attackSpeed); // +0x6c
    unsigned int moveSpeed   = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_moveSpeed);   // +0x68
    int mpRegenShort         = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_mpRegen);              // +0x66
    int hpRegenShort         = static_cast<int>(reinterpret_cast<MiciJobView const*>(sig)->m_hpRegen);              // +0x64
    unsigned int invenWeight = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_invenWeight); // +0x60
    // 第一次 blob_to_str(col=1, sig+0x3e, 0x22) → spec_property '%s'（ORIG 调用序在前）
    char const* specPropertyBlob =
        db->blob_to_str(1, const_cast<unsigned char*>(reinterpret_cast<MiciJobView const*>(sig)->m_specProperty), 0x22);
    // 第二次 blob_to_str(col=0, sig+0x36, 8) → element_resist '%s'
    char const* elementResistBlob =
        db->blob_to_str(0, const_cast<unsigned char*>(reinterpret_cast<MiciJobView const*>(sig)->m_elementResist), 8);
    unsigned int magDefense = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_magDefense);   // +0x34
    unsigned int magAttack  = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_magAttack);    // +0x32
    unsigned int phyDefense = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_phyDefense);   // +0x30
    unsigned int phyAttack  = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_phyAttack);    // +0x2e
    unsigned int maxMP      = static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_maxMp);        // +0x2a
    // P0：ORIG maxHP 无栈槽（080ec3bf 装载入 %edi 跨 NumberToString 保活）⇒
    // 不设具名局部，实参处内联表达式。

    db->set_query(
        "inSert into charac_info (m_id,charac_name,job,sex,maxHP,maxMP,phy_attack,"
        "phy_defense,mag_attack,mag_defense,element_resist,spec_property,"
        "inven_weight,hp_regen,mp_regen,move_speed,attack_speed,cast_speed,"
        "hit_recovery,jump,charac_weight,max_fatigue,create_time,expert_job,"
        "event_charac_level) values(%s,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',"
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW(),%d,%d)",
        NumberToString(sig->m_accId, 0), escapedName,
        (int)(signed char)sig->m_slot,     // job 列 ← sig+0x22（ORIG %ebx）
        (signed char)sig->m_ssnCheckFail,  // sex 列 ← sig+0x534c（ORIG %esi 无槽）
        static_cast<unsigned int>(reinterpret_cast<MiciJobView const*>(sig)->m_maxHp),  // +0x26（ORIG %edi）
        maxMP, phyAttack, phyDefense, magAttack, magDefense,
        elementResistBlob, specPropertyBlob,
        invenWeight, hpRegenShort, mpRegenShort, moveSpeed, attackSpeed,
        castSpeed, hitRecovery, jumpValue, expertJob, fatigueMax,
        sexAt0x25,                         // expert_job 列 ← sig+0x25（ORIG -0x60 槽）
        eventCharacLevel);                 // event_charac_level ← sig+0x23
}

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
#pragma pack(push, 1)
// +0x3a9 起点视图：CreateInventory 的 Inven_Item* 源（ORIG `lea 0x3a9(%eax),%edx`；
// 数组名退化在 4.4 恒折 add+mov）。
struct SigGiftInvenView
{
    unsigned char m_pre[0x3a9];
    Inven_Item m_inven;                // +0x3a9
};
#pragma pack(pop)

// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08400d2c 213=213。
// 修法：P102 同一 bool ok（声明在 sig 前取 -0xd 深槽）+ if(!ok) goto 三岛标签串（P223）
// + P248 档① SendResult int→bool（消 test/setne）+ SigGiftInvenView 具名成员取址复现
// ORIG `lea 0x3a9(%eax),%edx`（数组名退化恒折 add+mov）。
bool DB_CreateCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    // P102/P0：ORIG 全程复用同一 bool 槽 -0xd；声明在 sig 之前使 4.4 给 ok 更深槽。
    bool ok;
    SIG_CREATE_CHARAC* sig = stream->GetOutBuffer<SIG_CREATE_CHARAC>();
    if (sig == NULL)
    {
        return false;
    }

    sig->m_resultCode = 0;

    // P102/P0：ORIG 全程复用同一 bool 槽 -0xd（每次 `mov %al,-0xd; movzbl -0xd; xor $1;
    // test; je/jne`）⇒ 所有子调用结果赋给同一具名 bool ok，逐个判断；失败路径照 ORIG
    //（CheckLimit/CreateNewCharac/_CreateNewCharacStat 失败直落 SendResult，其余 rc=2）。
    ok = CheckLimitCreateNewCharac(sig, true);              // call 08401922
    if (!ok)
    {
        goto island1;                                       // ORIG jne nop+jmp 岛
    }
    ok = CreateNewCharac(sig);                              // call 08401dcc
    if (!ok)
    {
        goto island2;                                       // ORIG jne nop+jmp 岛
    }
    ok = GetCharacNo(sig);                                  // call 0840275c
    if (!ok)
    {
        sig->m_resultCode = 2;
        goto send_result;
    }
    ok = _CreateNewCharacStat(sig);                         // call 0840215e
    if (!ok)
    {
        goto island3;                                       // ORIG jne nop 岛（fall-through）
    }
    if (sig->m_giftFindOk != 0)                             // byte +0x533f
    {
        ok = CreateInventory(sig->m_characNo,
                             &reinterpret_cast<SigGiftInvenView*>(sig)->m_inven);
    }
    else
    {
        ok = CreateInventory(sig->m_characNo, NULL);
    }
    if (sig->m_equipFindOk != 0)                            // byte +0x5340
    {
        // ORIG lea 0x4e01(%eax)：&m_giftItem 起点，压缩长度 0x2dc。
        ok = CreateEquip(sig->m_characNo, &sig->m_giftItem);
    }
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    ok = CreateSkill(sig);
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    ok = CreatePvP(sig->m_characNo);
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    ok = _createCharacDungeon(sig->m_accId);
    if (!ok)
    {
        sig->m_resultCode = 2;
    }
    else
    {
        ok = _createCharacQuest(sig->m_characNo);
        if (!ok)
        {
            sig->m_resultCode = 2;
        }
        else
        {
            ok = _createCharacNPCRelationship(sig->m_characNo);
            if (!ok)
            {
                sig->m_resultCode = 2;
            }
            else
            {
                ok = InsertLimitCreateNewCharac(sig, true);
            }
        }
    }

island3:;
island2:;
island1:;
send_result:
    // P248 档①：ORIG 只有 `mov %al,-0xd`（无 test/setne）⇒ SendResult 返回类型改 bool。
    ok = SendResult(userIdx, slot, sig);
    if (!ok)
    {
        return false;
    }
    return true;
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
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401504 111=111。
// 修法：mktime 结果写回 nowSec 同槽（-0x20）+ P102 同一 bool ok（声明在 db 前取 -0x19）
// + P225 拆嵌套 if 消 && 物化 + 比较写 nowSec<=createTime（unsigned long 语义）复现 ja。
bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
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
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401922 158=158。
// 修法：bool 形参空 if 体复现头部 spill（-0x7c）+ P102 同一 bool ok（-0x29）+ lastAccessTime
// 先声明（-0x44/-0x48）+ P232 dead 槽 -0x20 + P227 全表达式临时链（%esi/%edi/%ebx）。
bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
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
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084011d4 120=120。
// 修法：P220/P222 正极性包裹（!=0 → setne/test/je return-true 岛）+ P227 两处全表达式
// 临时链（rowCnt→%ebx、sig+4→%esi、GetQueryString→%ebx）。
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
    // P220/P222 正极性包裹：ORIG `test; setne; test; je <return-true 岛>`。
    if (db->get_n_rows() != 0)
    {
        sig->m_resultCode = 2;
        // [b23S3C] P48 去缓存：无 nRows/badFetch 具名槽（ORIG rows 直判 + log 点
        // 二次 call；短路双 xor 直判无 al 落槽）；deleted!=0 反形（ORIG `test;je
        // gc块`、rc3 作 body，P191）；rowCnt/qs 在 ctor 前求值（ORIG +0x10c/
        // +0x17d call 位于 ctor 之前），值跨 ctor 4.1 落 %ebx/%esi 寄存器缓存
        // （P227 全表达式临时链复现，4.4 槽差已消）。
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
                // P227 全表达式临时链：ORIG rowCnt 于 ctor 前求值入 %ebx、
                // sig+4 入 %esi 跨 ctor 保活（084012eb/084012f0 段）。
                cMyTrace(__PRETTY_FUNCTION__, 0xd6a, 5)(
                    "already charac_name(%s) row(%d)", sig->m_name,
                    db->get_n_rows());
                // DAT_08c42c04 = "사이토"（ORIG 字符串常量）
                static const char BASELINE_NAME[] = "\xec\x82\xac\xec\x9d\xb4\xed\x86\xa0";
                if (strcmp(sig->m_name, BASELINE_NAME) == 0)
                {
                    // P227：GetQueryString 结果于 ctor 前入 %ebx（08401356 段）。
                    cMyTrace(__PRETTY_FUNCTION__, 0xd6d, 5)(
                        "already charac_name query(%s)", db->GetQueryString());
                }
            }
            sig->m_resultCode = 1;
        }
        return false;
    }
    return true;
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
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084016ae 110=110。
// 修法：P227 全表达式临时链（ORIG ctor 前右起求值 + %esi/%edi/%ebx 保活）。
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

    // P227 全表达式临时链：ORIG 在 ctor 前右起求值 varargs 并以 %esi/%edi/%ebx
    // 跨 ctor 保活（0840182b 段），具名对象形恒 ctor 后求值。
    cMyTrace(__PRETTY_FUNCTION__, 0xe76, 0)(
        "[LIMIT NEW CHARAC] %s,%d,%s,%d",
        NumberToString(sig->m_accId, 0),
        sig->m_characNo, dateBuf,
        G_CEnvironment()->get_server_group());
    return true;
}

// ===========================================================================
// InsertLimitCreateNewCharac(SIG*, bool) — 报告 @0x08401b9a
// count=count+1（无行插入 1），trace 行号 0xf61。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401b9a 141=141。
// 修法：空 if 体复现 bool 形参 spill（-0x16c）+ P227 全表达式临时链。
bool DB_CreateCharac::InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
                                                 bool traceFlag)
{
    // ORIG 头部 `mov 0x10(%ebp),%eax; mov %al,-0x16c(%ebp)`：bool 形参规范化
    // spill（空 if 体被 DCE，仅留 spill 两条；P232 死局部占槽律）。
    if (traceFlag)
    {
    }

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

    // P227 全表达式临时链（同 SIG* 重载）：ORIG ctor 前右起求值 + callee-saved 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0xf61, 0)(
        "[LIMIT NEW CHARAC] %s,%d,%s,%d",
        NumberToString(sig->m_accId, 0),
        sig->m_characNo, dateBuf,
        G_CEnvironment()->get_server_group());
    return true;
}

// ===========================================================================
// CreateNewCharac — 报告 @0x08401dcc
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=DIFF(N82 封顶) ORIG 0x08401dcc 234/237。
// 已收：`char escapedName[60]={0}` 复现 rep stos 清零（P214③）+ P227 全表达式临时链 trace
// + push edi/ebx 归位。残余：帧 0x70 vs 0x80、escapedName 槽 -0x68 vs -0x6c
// （P112/N82 跨版本槽分配平移）；语义/callsite 完整。
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
    // 于 env+0x68、STDBConnInfo m_db 在 +0x42；[b28d-Main·检查点18] 改成员直读
    //（原 get_db_conn_info() accessor 会发射 only-NEW 弱符号）。
    // [P183/P329 去缓存·ORIG 直证] ORIG 为 `movl $0x2,-0x24(%ebp)` + `call
    // G_CEnvironment; lea 0x68(%eax),%edx` + `call operator[]` + `add $0x42,%eax`
    // 全内联链式，**无** connMap/connInfo 具名缓存（我方原多两处 -0x10/-0xc 槽往返）。
    // 仅 connIdx 是 ORIG 实有的具名局部（-0x24 槽）。
    ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;       // 栈上局部初始值 2
    if (!_checkCharacName(db, G_CEnvironment()->m_dbConnInfo[connIdx].m_db, sig))
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

    // P214③/P312：ORIG 内联 `rep stos`（15 dword）清零 ⇒ 用 = {0} 形。
    char escapedName[60] = {0};
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
    // [P0 直证] ORIG index202 段为 `call <T>; mov %al,-0xd(%ebp); movzbl -0xd(%ebp),%eax;
    // xor $0x1,%eax`（具名 bool 局部槽往返 + 我方缺的 ext_movzbl）⇒ 补具名 ok。
    bool ok = db->exec(true);
    if (!ok)
    {
        // P227 全表达式临时链：ORIG NumberToString 于 ctor 前求值入 %ebx。
        cMyTrace(__PRETTY_FUNCTION__, 0x10e2, 5)(
            "DB_CreateCharac::CreateNewCharac, exec() ERROR m_id=%s",
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
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08402230 94=94。
// 修法：`char zipped[0x473e]={0}` 复现 4.1 memset call + size %edx 中转（破旧「待解」项）
// + P225 拆嵌套 if 消 && 物化 + P102 同一 bool ok + coin 内联实参（%ebx 跨 blob_to_str）。
bool DB_CreateCharac::CreateInventory(unsigned int characNo, Inven_Item* item)
{
    static const int handleIdx = 3;
    // N78/待解：ORIG 对 0x473e 大数组发 memset call 且 size 经 %edx 中转；
    // 试 `= {0}` 触发 4.1 同形（4.4 大数组可能同样退化为 call）。
    char zipped[0x473e] = {0};
    unsigned int zippedLen = 0x473e;
    // P225 推广：拆嵌套 if 消 4.4 的 && bool 物化（ORIG 两跳同一 L_skip）。
    if (item != NULL)
    {
        if (!compress_zip(zipped, zippedLen, reinterpret_cast<char*>(item), 0x3b55))
        {
            return false;
        }
    }

    // P0/P102：ORIG bool 槽 -0xd 两次复用（mov %al,-0xd; movzbl; xor; test; je）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    if (item == NULL)
    {
        // P0：ORIG call 结果直入实参槽（无具名 coin 局部）。
        db->set_query("inSert into inventory (charac_no, coin) values(%u, %d)",
                      characNo, WongWork::CEventMgr::GetCoinCharacCreate());
    }
    else
    {
        // P52/P227：最右实参先求值入 %ebx 跨 blob_to_str 保活（ORIG 084022eb）。
        db->set_query("inSert into inventory (charac_no, inventory,coin) values(%u, '%s',%d)",
                      characNo, db->blob_to_str(0, zipped, zippedLen),
                      WongWork::CEventMgr::GetCoinCharacCreate());
    }
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }

    db->set_query(
        "inSert into charac_inven_expand(charac_no,cargo,cargo_capacity,jewel) values(%u,'',0,'')",
        characNo);
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }
    return true;
}

// ===========================================================================
// CreateEquip — 报告 @0x084023ac
// item==NULL 直接成功；否则 zip(item, 0x2dc) upDate equipslot 列。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084023ac 88=88。
// 修法：P312 `char zipped[0x37a]={0}` + 非 4 对齐槽 -0x38e 复现内联清零慢路
// + P0 bool ok 声明在 db 前（-0xd/-0xc）。
bool DB_CreateCharac::CreateEquip(unsigned int characNo, Inven_Item* item)
{
    static const int handleIdx = 3;
    if (item == NULL)
    {
        return true;
    }

    // P312/N78：`char buf[N]={0}` + 非 4 对齐槽（-0x38e）复现 4.1 memset 内联慢路
    //（对齐剥离 2B + rep stos + 尾 2B/1B）。
    char zipped[0x37a] = {0};
    unsigned int zippedLen = 0x37a;
    if (!compress_zip(zipped, zippedLen, reinterpret_cast<char*>(item), 0x2dc))
    {
        return false;
    }

    // P0：ORIG `mov %al,-0xd` 槽往返 ⇒ 原码有具名 bool 局部（P168a① 裸形）；
    // 声明在 db 之前使 4.4 给 bool 更深槽（-0x10 尾 -0xd）、db 落 -0xc（同 ORIG）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate inventory set equipslot='%s' where charac_no = %u",
                  db->blob_to_str(0, zipped, zippedLen), characNo);
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }
    return true;
}

// ===========================================================================
// CreateSkill — 报告 @0x084024e6
// m_masteredSkillA/B 各 0x198 zip 后入 skill 表两 slot blob。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084024e6 148=148。
// 修法：两缓冲 `={0}`（-0x20d/-0x402 非 4 对齐慢路）+ P232 把 bool ok 声明最前占 -0x10 槽
// （同时下移缓冲 4B）+ lenSecond 先声明（-0x14/-0x18）。
bool DB_CreateCharac::CreateSkill(SIG_CREATE_CHARAC* sig)
{
    static const int handleIdx = 3;
    // P312/N78：`= {0}` + 非 4 对齐槽（-0x20d/-0x402）复现 4.1 内联清零慢路；
    // 声明序 bufSecond→bufSlot 使 4.4 槽位（浅→深）与 ORIG 一致。
    // P0/P232：ORIG 的 -0x10 槽即 exec 结果 bool（尾字节 -0xd，无初始化）；把它声明在
    // 最前占该槽，同时使两个缓冲下移到 -0x20d/-0x402（无需额外 dead 局部）。
    bool ok;
    char bufSecond[0x1f5] = {0};   // 帧 -0x20d 区（skillA 压缩输出，ORIG 501B）
    char bufSlot[0x1f5] = {0};     // 帧 -0x402 区（skillB 压缩输出，ORIG 501B）
    // ORIG 初始化序 -0x14 先/-0x18 后，且数组尾紧贴 lenSlot ⇒ lenSecond 先声明。
    unsigned int lenSecond = 0x1f5;   // -0x14
    unsigned int lenSlot = 0x1f5;     // -0x18

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
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }
    return true;
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
bool DB_CreateCharac::SendResult(int userIdx, int slot, SIG_CREATE_CHARAC* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x11db),
                       true);
    guard.operator*()->operator<<(userIdx);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);          // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x5354);  // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}