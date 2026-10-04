#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_OnTimeEventAckReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Inter_OnTimeEventAckReward.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "DNFFunctionLib.h"

#include <cstring>

// data 包缓冲的组件视图（P1：组件视图 → 单指令 disp 寻址，匹配 ORIG
// movzwl 0xa(%eax) / mov 0x10(%eax) 等；裸指针加偏移生成 add 两步）
#pragma pack(push, 1)
struct AckRewardView
{
    unsigned short m_packetId;    // +0x00
    unsigned short m_packetSize;  // +0x02
    unsigned short m_reversed1;   // +0x04
    unsigned int m_connNo;        // +0x06
    short m_resultNo;             // +0x0a
    char m_padC[4];               // +0x0c
    unsigned int m_accId;         // +0x10
    unsigned int m_charNo;        // +0x14
    unsigned int m_itemNo;        // +0x18
};
#pragma pack(pop)

#include "Inter_OnTimeEventAckReward/dispatch_sig.inc"


#include "Inter_OnTimeEventAckReward/SendOnTimeRewardByMail.inc"

