// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_MailBox_Req_System_Multi_Mail/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_System_Multi_Mail（OrigSysMail 批，4 ORIG 符号）
// 报告：docs/class_func_reports/DB_MailBox_Req_System_Multi_Mail/{dispatch,
//   _InsertLetter,_InsertPostal,sendResult}.md（逐指令转写；objdump 已核实地址）
// 依赖：
//   - SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL（SigTypes_D.h，m_data[0x3a9] pad）——
//     本 TU 以 pack(1) 镜像 SigSystemMultiMailView 按 ORIG 偏移访问。
//   - _InsertPostal 的元素域（stride 0x3d）：ORIG 以多套基址寻址同一元素——
//     seal `add $0x126` 单段、item 域 `add $0x120`+成员 disp、amp/blob 域
//     `add $0x130`+成员 disp、seperate `add $0x150; add $0x9`。元素基址
//     = sig+0x126，本 TU 以 SigPostalItem m_item[10] 元素视图统一具名访问；
//     ORIG 各基址与元素视图字段换算见逐字段注释。
//   - WongWork::DBCommon::GetIdentity 定义归 WongWorkEvents_A.cpp（TU 局部声明同款）。
// ============================================================================

#include "DB_MailBox_Req_System_Multi_Mail.h"

#include "SigTypes.h"       // SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL / SIG_MAILBOX_SEND_NEW_MAIL
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr / s_db_mgr / s_systemTime_
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"      // cMyTrace
#include "Inven_Item.h"     // Inven_Item / stAmplifyOption_t / UpgradeSeparateInfo

#include <ctime>
#include <cstdio>
#include <string>

namespace WongWork
{
namespace DBCommon
{
unsigned int GetIdentity(MySQL* db);   // _ZN8WongWork8DBCommon11GetIdentityEP5MySQL
}
}

// 元素内 amp 双视图（stAmplifyOption_t 0xe 字节与 guid blob 复用同一 0x14
// 字节区；_InsertPostal 报告 `lea 0x7(%eax)`（getValues）/`add $0xb`（blob0））。
#pragma pack(push, 1)
union SigPostalItem130Amp
{
    stAmplifyOption_t m_amp;   // +0x07（0x137，0xe 字节）
    struct
    {
        char m_ampHead[0x4];   // +0x07
        char m_guidBlob[0x10]; // +0x0b（blob_to_str(0) 源，10 字节）
    } m_g;
};

// 附件元素视图（stride 0x3d，元素基址 = sig + 0x126）。ORIG 基址换算（报告
// _InsertPostal 0x0844372e 逐指令）：view A 基 = 元素-0x06（`add $0x120`）；
// view B 基 = 元素+0x0a（`add $0x130`）；seperate `add $0x150; add $0x9` =
// 元素+0x33；Inven_Item 方法 this = 元素基址（`add $0x120; add $0x6` = 元素
// +0x00）；seal `add $0x126` 单段 = 元素+0x00。
struct SigPostalItem
{
    unsigned char m_sealFlag;         // +0x00（sig 0x126）
    char m_pad01;                     // +0x01
    int m_itemId;                     // +0x02（sig 0x128 = view A +0x08）
    char m_pad06;                     // +0x06
    int m_addInfo;                    // +0x07（sig 0x12d = view A +0x0d）
    unsigned short m_endurance;       // +0x0b（sig 0x131 = view A +0x11）
    int m_extendInfo;                 // +0x0d（sig 0x133 = view A +0x13）
    SigPostalItem130Amp m_ag;         // +0x11（sig 0x137 = view B +0x07）
    char m_randomBlob[0xe];           // +0x25（sig 0x14b = view B +0x1b）
    unsigned char m_seperateUpgrade;  // +0x33（sig 0x159 = view B +0x29）
    char m_rest[0x3d - 0x34];         // +0x34..0x3d
};

// SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 字段镜像（pack(1)，sizeof 0x3a9；偏移对照
// dispatch/_InsertLetter/_InsertPostal 报告逐指令 + ReqDBSendNewSystemMultiMail）。
struct SigSystemMultiMailView
{
    char m_letterText[0x1e];       // +0x00 邮件正文（_InsertLetter escape 源）
    char m_sendCharacName[0x100];  // +0x1e 发送者名（_InsertLetter escape 源）
    int m_recvCharacNo;            // +0x11e（letter.charac_no / postal.receive_charac_no）
    int m_sendCharacNo;            // +0x122（letter.send_charac_no / postal.send_charac_no）
    SigPostalItem m_item[10];      // +0x126（10 × 0x3d = 0x262）
    unsigned int m_count;          // +0x388 附件数量（_InsertPostal 循环上界）
    int m_gold;                    // +0x38c 附带金币
    unsigned int m_duration;       // +0x390 时段数（dispatch 折算到期）
    unsigned char m_unlimitFlag;   // +0x394
    unsigned int m_serverGroup;    // +0x395 ENUM_SERVER_GROUP
    char m_pad399[4];              // +0x399..0x39c（无访问证据）
    int m_letterId;                // +0x39d DB 回填 letter_id（GetIdentity）
    int m_occTime;                 // +0x3a1 到期时间（dispatch 写入）
    unsigned int m_stat;           // +0x3a5 letter.stat（与 (int) 实参异型 ⇒ 4.4 出
                                   // ORIG 的 `mov 0x3a5(%eax),%eax; mov %eax,%ebx` 两段，
                                   // b28c-34 探针 /tmp/c28l34_probe3.cpp q 实证）
};
#pragma pack(pop)
static_assert(sizeof(SigPostalItem) == 0x3d, "postal item stride drift (ORIG imul $0x3d)");
static_assert(sizeof(SigSystemMultiMailView) == 0x3a9, "SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL mirror drift");
static_assert(offsetof(SigSystemMultiMailView, m_item) == 0x126, "items zone base drift");

// sendResult 回复包（SIG_MAILBOX_SEND_NEW_MAIL 前 3 个 int）
struct SigMailboxSendReplyView
{
    int m_field00;
    unsigned int m_field04;   // 与形参 userIdx(int) 异型（NL7 定律）：4.4 RHS 先载翻 ORIG 形
    int m_field08;
};

// ============================================================================
// _InsertLetter —— ORIG 0x084435d4：escape(+0x1e 名)→0x1ff 缓冲、escape(+0x00
// 正文)→0x3d 缓冲 → set_query(letter insert 6 列) → exec → GetIdentity 回填
// +0x39d → 1。
// b28c-34（已翻正）：+0x3a5 stat 实参的 ORIG 两段装载
//   `mov 0x3a5(%eax),%eax; mov %eax,%ebx`（0x08443693/0x08443699）由 m_stat 视图
//   字段 unsigned int 化 + 保留 (int) 实参异型化复现（P211 异型化；探针
//   /tmp/c28l34_probe3.cpp q 实证同形）。语义结论（推断）：stat 原码类型不可由
//   二进制区分，异型化仅触发 4.4 的转换中转，store 宽度/偏移/mangled 不变。
// b28c-34 判词 N（N76 族：4.1 RHS 值先载 vs 4.4 LHS 地址先载）：尾部 +0x39d
//   回填 ORIG 0x08443711..0x08443719 为
//   `call GetIdentity; mov 0x10(%ebp),%edx; mov %eax,0x39d(%edx)`；
//   4.4 对 cast 视图 LHS 恒先求地址并缓存 callee-saved：
//   `mov 0x10(%ebp),%ebx; …; call GetIdentity; mov %eax,0x39d(%ebx)`。
//   已试形 20+（探针 /tmp/c28l34_probe{1..10}.cpp）：-> / (*p). / p[0]. /
//   (V&)*p / reinterpret_cast / *(int*)&视图 / 语句表达式（整体与 RHS）/
//   条件式 / 逗号 / 返回赋值结果 / always_inline setter / throw()/nothrow 声明
//   —— 全部 LHS-first；唯一复现 ORIG 形的写法是「形参具名成员」直访问
//   （探针 s_uint：`call; mov 0x8(%ebp),%edx; mov %eax,0x4(%edx)`），而共享头
//   SigTypes_D.h 的 SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL 仅有 m_data[0x3a9] pad，
//   翻正需改共享头（本任务约束 5 禁止）⇒ 保持语义正确的 cast 视图访问。
// [b28d-N76] fresh 复验 105=105：残差 = 上述 LHS-first 装载序（唯一差异行），
// 已试形 20+ 均不可择（P211 边界：已异型件直接封顶）⇒ 维持 N76。
// [b28d-34R] 字段命名修正：letter 列 charac_no/send_charac_no 的 ORIG 实参序
//   （`mov 0x11e(%eax),%eax; … mov 0x122(%eax),%edx`，0x084436b0/0x084436a7）
//   确证 0x11e=收件人、0x122=发件人（与 _InsertPostal 的 postal 列互证）；
//   视图字段已按此改名，值序不变 ⇒ 本件汇编零变化。
// ============================================================================
#include "DB_MailBox_Req_System_Multi_Mail/_InsertLetter.inc"
}

// ============================================================================
// _InsertPostal —— ORIG 0x0844372e：std::string 拼 "(...)" 20 列（循环 count
// 个附件，i!=0 时 gold 清 0；每项 getValues/GetUpgradeSeparate/blob_to_str 等）
// → set_query(postal insert ... values %s) → exec 失败 cMyTrace → 1。
// [b28d-34R 真 bug 修复] ORIG sprintf 变参 2/3 位序 = sig+0x122→0xc(%esp)、
//   sig+0x11e→0x10(%esp)（0x08443a32/0x08443a2e），即 postal.send_charac_no=
//   发件人 0x122、receive_charac_no=收件人 0x11e。旧视图字段名 send/recv 互换，
//   旧代码把 0x11e 填进 send 列、0x122 填进 receive 列（列值互换的真语义 bug）；
//   已按 0x11e=recv / 0x122=send 改名（三处消费点同步）⇒ 该装载段与 ORIG 逐条对齐。
//   另两处形态修复：`char buf[0x400]` 声明下移至 getValues 之后（ORIG rep stos
//   在 getValues 之后）、ampType/ampValue 改循环体内声明（4.4 槽 -0x31/-0x34 对齐）。
// [残余 N] fresh 复验 269/276（clean 口径）：元素域寻址 4.4 恒 base-first
//   （`mov 0x10(%ebp),%eax; … lea (%edx,%eax,1)`）vs ORIG index-first
//   （`mov -0x1c(%ebp),%eax; … add 0x10(%ebp),%eax`），6 处同形；已试 union→
//   纯数组视图、`(m_item+i)->` 指针算术两形均不可翻（后者反多 1 条 lea）
//   ⇒ P30/N102/N4 族封顶。调用点差集仅 P93 别名（string/cMyTrace C1↔C2、D1↔D2 同址）。
// ============================================================================
#include "DB_MailBox_Req_System_Multi_Mail/_InsertPostal.inc"
}

// ============================================================================
// dispatch —— ORIG 0x0844346a（虚覆盖）：GetOutBuffer<SIG_MAILBOX_SEND_SYSTEM_
// MULTI_MAIL> → GetDBHandle(DB_HANDLE_3, +0x395) → getCurSec → localtime_r
// (+1h/min/sec=0) → mktime → 按 +0x390 时段折算 → 写 +0x3a1 → _InsertLetter →
// _InsertPostal（失败返 0）→ sendResult(+0x11e = m_recvCharacNo, +0x395) → 1。
// ============================================================================
#include "DB_MailBox_Req_System_Multi_Mail/dispatch.inc"
}

// ============================================================================
// sendResult —— ORIG 0x08443b6e：Acquire("DBThread.cpp",0xa5be) → CStreamGuard
// (stream,true) → <<0x80(int) → <<-1(int) → GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>
// → 写 +0x00=0/+0x04=userIdx/+0x08=group → put(DISPATCH_Q, guard) → 1。
// ============================================================================
#include "DB_MailBox_Req_System_Multi_Mail/sendResult.inc"
}