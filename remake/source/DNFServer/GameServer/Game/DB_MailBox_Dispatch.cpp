// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_MailBox_Dispatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_MailBox 域 DB 侧 dispatch 任务（OrigMailWave18 批，
// 独占 TU，未入 CMake，待 Main 登记）
// 覆盖 7 个 ORIG T 符号（地址见 DB_MailBox_Dispatch.h 头注释）：
//   1. getList2inQuery(uint*, char*)                    0x083ff968
//   2. DB_MailBox_Req_Delete::dispatch                 0x0841f084
//   3. DB_MailBox_Req_CheckReceiver::dispatch          0x0841c882
//   4. DB_Multi_MailBox_Req_Send::dispatch             0x0843bada
//   5. DB_Multi_MailBox_Req_Send::_sendResult          0x0843bc16
//   6. DB_ReqQueryCharacInfoMailbox::dispatch          0x0843bd46
//   7. DB_ReqQueryCharacInfoMailbox::_sendResult       0x0843c224
// 报告均已全文阅读后转写；SIG 布局：SIG_MAILBOX_DEL_MAIL / SIG_MAILBOX_LIST /
//   SIG_MAILBOX_CHECK_RECEIVER / SIG_QUERY_CHARAC_INFO（2026-09-05 §4.3 已字段化，
//   本 TU 全部具名字段访问）。m_payload 透传区内 +0xaa 标题串（= this+0xdb）无字段化
//   证据，用 `(const char*)((unsigned int)sig + 0xdb)` 整型加法形复现 ORIG 0x0843bb71
//   两步 `add $0xdb,%eax; mov %eax,%ecx`（P319；数组退化在 4.4 恒折单条 lea）。
// 依赖：DB_MailBox_Req_Send::InsertLetter（DB_MailBox_Req_Send.h，已实现）；
//   NumberToString 定义归 LinkFinalDeps.cpp；DBDispatcher::dispatch 归 DB_Login.cpp。
// [b28d-SweepDBMailBoxDispatch 循环/goto 专项] 本 TU 无「循环内 goto 退出标签」模式：
//   仅 2 个 for 循环 + 1 个函数级 goto done（QueryCharacInfo::dispatch 尾部 return 1）。
//   jmp/jcc 目标**指令下标**逐位核对（全部 29 共同符号 + 5 主函数）MATCH=True；
//   循环回边：getList2inQuery `jne`(src29→tgt12)、CheckReceiver::dispatch
//   `jne`(src145→tgt86)，均 BACKWARD 指向循环体，与 ORIG 逐位一致。
// ============================================================================

#include "DB_MailBox_Dispatch.h"

#include "SigTypes.h"       // SIG_MAILBOX_DEL_MAIL / SIG_MAILBOX_CHECK_RECEIVER / SIG_QUERY_CHARAC_INFO
#include "GlobalData.h"     // s_db_mgr / s_systemTime_ / s_stream_pool / s_big_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"      // cMyTrace
#include "LogManager.h"
#include "DNFFunctionLib.h" // NumberToString
#include "DB_MailBox_Req_Send.h"  // DB_MailBox_Req_Send::InsertLetter / ENUM_LETTER_STAT

#include <ctime>
#include <cstring>
#include <cstdio>

// ============================================================================
// 1. getList2inQuery —— ORIG 0x083ff968（_lt_global_gt_/getList2inQuery.md）：
//    out[0]=0 → memcpy(out,"in (",5) → for(i=1; i<list[0]; ++i)
//    sprintf(out,"%s%d,",out,list[i]) → sprintf(out,"%s%d)",out,list[i]) → out。
// ============================================================================
char* getList2inQuery(unsigned int* list, char* out)
{
    out[0] = '\0';
    memcpy(out, "in (", 5);
    unsigned int i;
    for (i = 1; i < list[0]; ++i)
    {
        sprintf(out, "%s%d,", out, list[i]);
    }
    sprintf(out, "%s%d)", out, list[i]);
    return out;
}

// ============================================================================
// 2. DB_MailBox_Req_Delete::dispatch —— ORIG 0x0841f084（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_DEL_MAIL> → GetDBHandle(DB_HANDLE_3,0)；
//    count==0 返 0；count==1 单条 upDate（postal_id=%d）；否则
//    getList2inQuery 生成列表串（postal_id %s）；exec(true) 失败返 0，成功返 1。
//    局部 static char strQuery[]（ORIG _ZZN21DB_MailBox_Req_Delete8dispatch
//    EiiP6StreamE8strQuery .bss 证据）。
// ============================================================================
#include "DB_MailBox_Dispatch/dispatch.inc"
}

// ============================================================================
// 3. DB_MailBox_Req_CheckReceiver::dispatch —— ORIG 0x0841c882（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 双 DB 句柄(2/3) →
//    charac_info 按收件人名查 charac_no/lev/delete_flag/m_id（循环读全部行）→
//    BigStreamPool 回包：<<0x7e <<slot，回填 +0x1d/+0x21/+0x25/+0x29，
//    memcpy(+0x31,0x3f88) 载荷透传 → 黑名单 count(*)>0 时 +0x3fb9=0x4d → put。
// b28d-SweepDBMailBoxDispatch（AE，266=266，jcc 序列逐位全等）：三处真差异已修——
//   ① SQL 串补具名 char* 局部（ORIG 0x0841c8f8 存 -0x24、0x0841c980 回读；P244）；
//   ② NumberToString 内联进 set_query 实参位（ORIG 0x0841cb73 先求值 sender 入 %ebx
//      跨 call 保活，cdecl 右→左；P329）；
//   ③ 黑名单判定改嵌套 if（ORIG 0x0841cbd1/cbe0/cbfe/cc05 四个独立 je 同出口；P225）。
//   另删 sender 实参冗余 (int) cast（P265：unsigned 字段+异型 cast 致 mem→eax→ebx 两段，
//   ORIG 为单段 `mov 0x2d(%eax),%ebx`）。
// ============================================================================
#include "DB_MailBox_Dispatch/dispatch_1.inc"
}

// ============================================================================
// 4. DB_Multi_MailBox_Req_Send::dispatch —— ORIG 0x0843bada（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 到期时间 = 下一整点
//    （getCurSec+1h 清零分秒 → mktime）→ GetDBHandle(DB_HANDLE_3,0) →
//    InsertLetter(db, +0x1d 收件人, +0x2d 发件人, +0x3fc1 name, +0xdb title,
//    +0x3fbd letterId, t, 0)，失败 logFormat(0x8e53) → _sendResult(slot, sig)
//    → 返 1。
// b28c-L20: N1 — 唯一真残差 `sig->m_payload + 0xaa`（m_payload char[0x3f88] @+0x31 ⇒
//   +0xdb）：ORIG 0x0843bb71 `add $0xdb,%eax; mov %eax,%ecx` 两步，4.4 恒折单条
//   `lea 0xdb(%eax),%esi`（数组退化+常量折叠），其余为寄存器名对调（N82附）。ndiff 13。
// ============================================================================
#include "DB_MailBox_Dispatch/dispatch_2.inc"
}

// ============================================================================
// 5. DB_Multi_MailBox_Req_Send::_sendResult —— ORIG 0x0843bc16（_sendResult.md）：
//    BigStreamPool::Acquire("DBThread.cpp",0x8e67) → CStreamGuard(stream,true) →
//    <<0x1f5 → cMyTrace("bool ..._sendResult(int, SIG_MAILBOX_CHECK_RECEIVER*)",
//    0x8e6a,0)("E_MULTI_MAILBOX_REQ_SEND : %d",0x1f5) → <<slot →
//    GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER> → memcpy 0x3fde → put(DISPATCH_Q) → 1。
// b28c-L20: AE — 全表达式临时链 cMyTrace(...)(...) 复现 ORIG 槽方向（guard -0x24 深 /
//   trace -0x1c 浅；具名 trace 时 4.4 反向，P221/P227）。
// ============================================================================
#include "DB_MailBox_Dispatch/_sendResult.inc"
}

// ============================================================================
// 6. DB_ReqQueryCharacInfoMailbox::dispatch —— ORIG 0x0843bd46（dispatch.md 全文）：
//    基类 DBDispatcher::dispatch 失败返 0 → GetDBHandle(DB_HANDLE_2,0) →
//    GetOutBuffer<SIG_QUERY_CHARAC_INFO>（首字节即角色名）→ escape_string →
//    charac_info 查 7 列；exec 失败/无行/取行失败 → _sendResult(2,...)；
//    delete_flag!=0 → _sendResult(0x15,...)；成功 → 读列 1..6 →
//    _sendResult(0, slot, sig, characNo, job, lev, growType, sex) → 返 1
//    （ORIG +0x377 第 3 参 name 传 sig=GetOutBuffer 指针，非转义缓冲）。
//    注意 ORIG 对 get_n_rows()==0 有两次独立判定（P48 双调用），照写。
//    b28d-SweepDBMailBoxDispatch（AE，227=227，jcc 序列逐位全等）：4 处失败路径改
//    `return _sendResult(...)`，复现 ORIG 0x0843be5b/bebe/bf6f/bfc5 的 `call; jmp +0x3e6`
//    （跳过 mov $1）⇒ _sendResult 返回类型随之改 bool（P292 探针 f/g 实证：int 返回
//    会多 test/setne 归一化）。成功路径仍 goto done 共享单一置位（ORIG 0x0843c0db
//    `jmp +0x3e1` 到 mov $1）。
// ============================================================================
#include "DB_MailBox_Dispatch/dispatch_3.inc"
}

// ============================================================================
// 7. DB_ReqQueryCharacInfoMailbox::_sendResult —— ORIG 0x0843c224（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x8ed6) → CStreamGuard(stream,true) →
//    <<0x1f6 → <<slot → GetInBuffer<SIG_QUERY_CHARAC_INFO> → +0x20=errCode；
//    errCode==0 时 strncpy(name,0x1d)、+0x24=characNo、+0x28=(char)job、
//    +0x2a=(short)lev、+0x2c=(char)growType、+0x2d=(char)sex → put → 返 1。
// ============================================================================
#include "DB_MailBox_Dispatch/_sendResult_1.inc"
}