// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCharacInformNotice/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// ORIG 域常量（nm：_ZN12InformNoticeL18kInformNoticeIndexE / L16kInformNoticeBitE，
// r 段内部链接；m_flags[16] × 8bit 标志板）
namespace InformNotice
{
static const int kInformNoticeIndex = 16;
static const int kInformNoticeBit = 8;
}
#include "CCharacInformNotice.h"
#include "CUser.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include <string.h>

// ============================================================================
// [b28d-solo-CCharacInformNotice] 本 TU 判词表（verdict = check_tu_routed.sh fresh .o）：
//   checkInformNoticeFlag 0x080e4892 FIXED→AE（33=33）：P56 if-return 形复现 ORIG
//     尾部 `xor$1; test%al; je; mov$1; jmp; mov$0` 常量岛；.o 残余仅未重定位分支
//     目标显示（P264）。
//   handleInformNoticeFlag 0x080e4806 DIFF(55/54) N 封顶：P223 三岛形（`!f()` 负
//     条件 goto 连续尾标签 L1/L2/L3，逐岛复现）；残差①第三守卫 4.4
//     `xor$1;test%al;jne` vs ORIG `test%al;je`（P168a①/P292 bool 归一不可造；
//     正向 if 可消 xor 但 4.4 把 if-false 落点前置并 jump-thread 岛2/岛3，51<55 更劣）；
//     ②两处 N18a 反向折叠（ORIG movzbl→edx→ebx、lea→edx→ecx 中转，4.4 恒单段）。
//     P248 三档试尽：m_flags 元素改 1 字节 enum 仅翻第一处中转（/tmp/cn_probe4.cpp
//     v10/v13/v15），第二处与 xor 在 12 形下均不可翻（/tmp/cn_probe5..9.cpp）；
//     4.4.6 后端复测同形（非 N7 后端敏感族）。P167 CLEAR(2/2)。
// ============================================================================


#include "CCharacInformNotice/CCharacInformNotice.inc"


#include "CCharacInformNotice/dtor_CCharacInformNotice.inc"


#include "CCharacInformNotice/_reset.inc"


#include "CCharacInformNotice/checkInformNoticeFlag.inc"


#include "CCharacInformNotice/handleInformNoticeFlag.inc"


#include "CCharacInformNotice/getInformNoticeFlag.inc"


#include "CCharacInformNotice/getInformNoticeFlagIndex.inc"


#include "CCharacInformNotice/sendInformNoticeFlag.inc"


#include "CCharacInformNotice/loadData.inc"


#include "CCharacInformNotice/_saveData.inc"


#include "CCharacInformNotice/getData.inc"

