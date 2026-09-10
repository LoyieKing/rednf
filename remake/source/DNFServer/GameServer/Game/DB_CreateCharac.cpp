// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_CreateCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// b28d-solo-DB_CreateCharac verdict=DIFF(N7 封顶) ORIG 0x080ec2a0 orig/ours=154/153。
// 已收：①blob_to_str 调用序（先 col=1 spec 后 col=0 elem）②P183 去 jv 指针缓存（每个字段
// 重读 sig 形参 `mov 0x10(%ebp),%eax; mov off(%eax)`，与 ORIG 逐条一致）③maxHP 不设局部、
// 实参内联（%edi 承载，push/pop %edi 与帧 0xcc 归位）④+0x25 改具名局部 -0x60、+0x534c 内联
// （ORIG 前者有槽、后者 %esi 无槽）⑤帧/槽位/-0x64..-0x1c 全对齐、%edi/%esi/%ebx 角色一致。
// 残余 = 1 条：ORIG `call NumberToString; mov %eax,%edx`（结果移 %edx，压栈用 %eax 中转），
// 4.4 恒留 %eax、改用 %edx 中转压栈 ⇒ 助记符序列全等仅中转寄存器角色互换（N7 族；试过
// P226 裸语句表达式/具名局部/register 提示/cast 四形，均不翻）。语义/callsite 完整。
#include "DB_CreateCharac/MakeInsertCharacInfo.inc"
}

// ===========================================================================
// C1/C2 — DB_CreateCharac.md @0x08400cc2：基类 ctor + 设 vptr。
// D1/D2/D0 — ~.md @0x08400cde/@0x08400d0e：设 vptr + 基类析构；D0 另 delete。
// 非 inline 定义发射全套生命周期符号。
// ===========================================================================
#include "DB_CreateCharac/DB_CreateCharac.inc"
}

#include "DB_CreateCharac/dtor_DB_CreateCharac.inc"
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
#include "DB_CreateCharac/dispatch.inc"
}

// ===========================================================================
// _createCharacNPCRelationship — 报告 @0x0840103e
// seLect/inSert charac_npc 一条；尾部 exec 返回值即调用方判定值。
// ===========================================================================
#include "DB_CreateCharac/_createCharacNPCRelationship.inc"
}

// ===========================================================================
// _createCharacQuest — 报告 @0x08401094
// ===========================================================================
#include "DB_CreateCharac/_createCharacQuest.inc"
}

// ===========================================================================
// _createCharacDungeon — 报告 @0x084010fc：无行则补插空 member_dungeon 行。
// ===========================================================================
#include "DB_CreateCharac/_createCharacDungeon.inc"
}

// ===========================================================================
// CheckLimitCreateNewCharac(SIG*) — 报告 @0x08401504
// 当日（本地零点起）已建 >1 角色拒建（rc=5）；exec 失败 rc=2。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401504 111=111。
// 修法：mktime 结果写回 nowSec 同槽（-0x20）+ P102 同一 bool ok（声明在 db 前取 -0x19）
// + P225 拆嵌套 if 消 && 物化 + 比较写 nowSec<=createTime（unsigned long 语义）复现 ja。
#include "DB_CreateCharac/CheckLimitCreateNewCharac.inc"
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
#include "DB_CreateCharac/CheckLimitCreateNewCharac_1.inc"
}

// ===========================================================================
// makeRequestResetLimitCreateMid — 报告 @0x08401864
// channel_no==1 时经内部包请求重置中端限建表（==1 布局，匹配 ORIG sete 形式）。
// ===========================================================================
#include "DB_CreateCharac/makeRequestResetLimitCreateMid.inc"
}

// ===========================================================================
// _checkCharacName — 报告 @0x084011d4
// 重名/delete_flag 判定；gc_no==1010(0x3f2) 且恰与基准名相同时追查 query 日志。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084011d4 120=120。
// 修法：P220/P222 正极性包裹（!=0 → setne/test/je return-true 岛）+ P227 两处全表达式
// 临时链（rowCnt→%ebx、sig+4→%esi、GetQueryString→%ebx）。
#include "DB_CreateCharac/_checkCharacName.inc"
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
#include "DB_CreateCharac/_checkCreateEventCharacter.inc"
}

// ===========================================================================
// _updateCreateEventCharacter — 报告 @0x08401428
// 同事件窗内置 member_login.event_charac_flag=1 并回写角色标志。
// 52/52 逐条对齐：窗口右值内联（P0/P183）、GetDBHandle(6,0)+
// NumberToString(m_accId)+set_query+exec(true)、失败 rc4=2@0x5344、
// 成功 flag=1@0x24 返 1、窗外 rc4=4 块置函数尾（与 ORIG 同序）。
// 残余文本差均为 call/jmp 重定位与数据地址占位（§6.3），relink 后转 AE。
// ===========================================================================
#include "DB_CreateCharac/_updateCreateEventCharacter.inc"
}

// ===========================================================================
// InsertLimitCreateNewCharac(SIG*) — 报告 @0x084016ae：新行插入 + trace。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084016ae 110=110。
// 修法：P227 全表达式临时链（ORIG ctor 前右起求值 + %esi/%edi/%ebx 保活）。
#include "DB_CreateCharac/InsertLimitCreateNewCharac.inc"
}

// ===========================================================================
// InsertLimitCreateNewCharac(SIG*, bool) — 报告 @0x08401b9a
// count=count+1（无行插入 1），trace 行号 0xf61。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401b9a 141=141。
// 修法：空 if 体复现 bool 形参 spill（-0x16c）+ P227 全表达式临时链。
#include "DB_CreateCharac/InsertLimitCreateNewCharac_1.inc"
}

// ===========================================================================
// CreateNewCharac — 报告 @0x08401dcc
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=DIFF(N82 封顶) ORIG 0x08401dcc 234/237。
// 已收：`char escapedName[60]={0}` 复现 rep stos 清零（P214③）+ P227 全表达式临时链 trace
// + push edi/ebx 归位。残余：帧 0x70 vs 0x80、escapedName 槽 -0x68 vs -0x6c
// （P112/N82 跨版本槽分配平移）；语义/callsite 完整。
#include "DB_CreateCharac/CreateNewCharac.inc"
}

// ===========================================================================
// _CreateNewCharacStat — 报告 @0x0840215e
// ===========================================================================
#include "DB_CreateCharac/_CreateNewCharacStat.inc"
}

// ===========================================================================
// CreateInventory — 报告 @0x08402230
// item==NULL 仅写 coin；否则将 item 起点 0x3b55 字节 zip 后入库 + 空背包扩容表。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08402230 94=94。
// 修法：`char zipped[0x473e]={0}` 复现 4.1 memset call + size %edx 中转（破旧「待解」项）
// + P225 拆嵌套 if 消 && 物化 + P102 同一 bool ok + coin 内联实参（%ebx 跨 blob_to_str）。
#include "DB_CreateCharac/CreateInventory.inc"
}

// ===========================================================================
// CreateEquip — 报告 @0x084023ac
// item==NULL 直接成功；否则 zip(item, 0x2dc) upDate equipslot 列。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084023ac 88=88。
// 修法：P312 `char zipped[0x37a]={0}` + 非 4 对齐槽 -0x38e 复现内联清零慢路
// + P0 bool ok 声明在 db 前（-0xd/-0xc）。
#include "DB_CreateCharac/CreateEquip.inc"
}

// ===========================================================================
// CreateSkill — 报告 @0x084024e6
// m_masteredSkillA/B 各 0x198 zip 后入 skill 表两 slot blob。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084024e6 148=148。
// 修法：两缓冲 `={0}`（-0x20d/-0x402 非 4 对齐慢路）+ P232 把 bool ok 声明最前占 -0x10 槽
// （同时下移缓冲 4B）+ lenSecond 先声明（-0x14/-0x18）。
#include "DB_CreateCharac/CreateSkill.inc"
}

// ===========================================================================
// CreatePvP — 报告 @0x084026ec
// ===========================================================================
#include "DB_CreateCharac/CreatePvP.inc"
}

// ===========================================================================
// GetCharacNo — 报告 @0x0840275c：@@identity → sig->m_characNo。
// ===========================================================================
#include "DB_CreateCharac/GetCharacNo.inc"
}

// ===========================================================================
// SendResult — 报告 @0x084027b0
// 大流包：userIdx、slot、SIG 全量 0x5354 二进制 → DISPATCH_Q。
// ===========================================================================
#include "DB_CreateCharac/SendResult.inc"
}