// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Inter_OnTimeEventAckReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
struct AckRewardView
{
    char m_pad[0xa];        // +0x00
    short m_resultNo;       // +0x0a（signed：ORIG movzwl+movswl）
    char m_pad2[0x4];       // +0x0e
    unsigned int m_accId;   // +0x10
    unsigned int m_charNo;  // +0x14
    unsigned int m_itemNo;  // +0x18
};

// ---------------------------------------------------------------------------
// Inter_OnTimeEventAckReward::dispatch_sig(CUser*, char*, int)
//   ORIG 0x084e11ec（T，size 0x38b）。vtable[0] 虚槽（ISigDispatcher 纯虚覆盖）。
//   data 指向客户端 ACK 包缓冲（+0x0a short result_no；+0x10/+0x14/+0x18 三个 uint）。
//   全程返回 0；日志走 cMyTrace 临时链式（P37：args 提前求值存 ebx/push 帧，
//   栈槽从低地址按词法序分配——与 ORIG -0x6c..-0x1c 逐槽对应）。
// ---------------------------------------------------------------------------
// [b28d-solo-Inter_OnTimeEventAckReward] verdict=AE（check_tu_routed.sh clean 口径；p167 27/27 CLEAR）。
//   修法：get_charac_no 比较点**字段侧删 (int) cast**、右侧同型化 (unsigned int)：
//   ORIG 单条 `mov 0x10(%eax),%ebx` 直落 callee-saved 复现；4.4 对「unsigned 字段 + (int) cast」
//   恒发 `mov mem,%eax; mov %eax,%ebx` 两段中转（探针 /tmp/b28d_s309_probe.cpp f1 vs f3）。
//   变参预装：全表达式临时链形（P227）已复现 ORIG ctor 前 %edi/%esi/%ebx 保活，非 N82。
#include "Inter_OnTimeEventAckReward/dispatch_sig.inc"
}

// ---------------------------------------------------------------------------
// Inter_OnTimeEventAckReward::SendOnTimeRewardByMail(uint, uint, uint)
//   ORIG 0x084e1578（T，size 0x17c）。非虚。nCharacNo=0xc 槽（邮件接收角色号）、
//   itemNo=0x10 槽（find_item 参数）、count=0x14 槽（create_item 数量）。
//   Inven_Item 构造在 find_item 判空之后（ORIG +0x02b）；item.m_addInfo(+0x02)
//   即 Ghidra 的 local_60@-0x5c（create_item 后仍读该字段）。
// ---------------------------------------------------------------------------
#include "Inter_OnTimeEventAckReward/SendOnTimeRewardByMail.inc"
}
