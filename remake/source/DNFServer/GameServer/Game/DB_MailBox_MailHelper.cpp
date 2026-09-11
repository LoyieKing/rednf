// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_MailBox_MailHelper/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "DB_MailBox_MailHelper.h"

#include "SigTypes.h"       // SIG_MAILBOX_LIST
#include "CMailBoxHelper.h" // SIG_LETTER_INFO（0x12b）

#include <vector>
#include <algorithm>

#pragma pack(push, 1)
// SIG_LETTER_INFO 内 +0x125 字段（occ_time）；必须在 pack(1) 内，
// 否则默认 4 字节对齐会把 m_occ 推到 +0x128（与 ORIG 0x125 不符）。
struct LetterOccView
{
    char m_pad[0x125];
    int  m_occ;              // +0x125
};
#pragma pack(pop)

#include "DB_MailBox_MailHelper/RemoveMailByLatestLetter.inc"

#include "DB_MailBox_MailHelper/GetOnlyMailCount.inc"


#include "DB_MailBox_MailHelper/RemoveMailByMailIndex.inc"


#include "DB_MailBox_MailHelper/FindLastOccTimeMail.inc"


#include "DB_MailBox_MailHelper/CompareLetter.inc"


#include "DB_MailBox_MailHelper/ArrangeMailAndLetter.inc"

