// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ServerXml/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "ServerXml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "AuctionItem.h"
#include "DNFFunctionLib.h"

namespace np_server_xml
{

// ORIG：命名空间内静态 FILE_NAME（_ZN13np_server_xmlL9FILE_NAMEE），
// StrLoading 与 Load Fail 日志引用该符号。
static const char FILE_NAME[] = "server_str.xml";

}  // namespace np_server_xml

// [T1-GI · P327] 键名位：ORIG `_GLOBAL__I_g_ServerString_`（0x80c6a95）⇒ ORIG 源码中本对象
// 定义先于本 TU 所有函数定义（P327 选名 = 首个「强符号 · 外部链接 · 本 TU 定义」实体；
// 非限定全局对象 ⇒ 裸名），故对象定义前移至此。
// 本件属「带动态初始化对象」（ORIG 0x80c6a2c / 我方 static_init 均调 CServerXml C1 +
// __cxa_atexit），默认受 P327 移动禁令；此处经实测豁免：改前/改后
// `__static_initialization_and_destruction_0` 体**逐字节相同**（ioinit 由行 6 <iostream> 定序，
// 恒在其前；ORIG 亦为 ioinit → CServerXml 同序）、.text/.data/.bss/.ctors 段字节全同、
// 符号集合仅 GI 名一项变化 ⇒ 构造序与可观察行为零变化。
// 实测：键名 `_GLOBAL__I_g_ServerString_` == ORIG；四数 68/88/4/1 → 69/88/4/1（missing 45→44）。
np_server_xml::CServerXml g_ServerString_;

namespace np_server_xml
{

#include "ServerXml/CServerXml.inc"

#include "ServerXml/dtor_CServerXml.inc"

#include "ServerXml/InitString.inc"

#include "ServerXml/StrLoading.inc"

#include "ServerXml/StrLoading_1.inc"

// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族）：
//   ORIG 0x080c5e92: mov (%eax),%edx   ; vptr+0x2c 槽 → 目标 %edx
//   ORIG 0x080c5e9a: call *%edx        ; this 载 %eax
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax（目标 %eax、this %edx）
// 助记符序列与指令数全等（100/100），仅寄存器角色由后端决定 ⇒ 源码无解
// （N7 判词：临时/引用/限定名三形均无效，8 件实证）。
// 修复路径 = 把本 TU 路由到 C6446R（cc1plus 4.4.6）。P230 补：4.4.6 间接 call 目标用 %edx，
// 与 ORIG 4.1 同形。实测（同源码、仅换后端，compare_tu_game_orig 口径）：
//   identical 69 / ae 88 / near 4 / diff 1  →  identical 69 / ae 92 / near 0 / diff 1
//   （本件 NEAR→AE；同批 EventLoad 110/110、RGBALoad 93/93、ProcessLoad 83/83 一并归零）
// 路由表 source/toolchain/cmake/dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落
//   `*ServerCommon/ServerXml.cpp) exec "$C6446R" "$@"`。
// ============================================================================
#include "ServerXml/CharsetInit.inc"

// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族），本件 3 处：
//   ORIG 0x080c604c / 0x080c60c8 / 0x080c610e: call *%edx（前置 mov (%eax),%edx）
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax
// 助记符序列与指令数全等（110/110），仅寄存器角色由后端决定 ⇒ 源码无解（N7 判词）。
// 修复路径 = 本 TU 路由到 C6446R（cc1plus 4.4.6；P230 补：4.4.6 间接 call 目标 = %edx）。
// 实测（同源码仅换后端）：ServerXml.cpp identical 69 / ae 88 / near 4 / diff 1
//   →  identical 69 / ae 92 / near 0 / diff 1（本件 NEAR→AE）。
// 路由表 dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落 *ServerCommon/ServerXml.cpp 路由。
// ============================================================================
#include "ServerXml/EventLoad.inc"

// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族），本件 4 处：
//   ORIG 0x080c61c3 / 0x080c61f0 / 0x080c621d / 0x080c624a: call *%edx（前置 mov (%eax),%edx）
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax
// 助记符序列与指令数全等（93/93），仅寄存器角色由后端决定 ⇒ 源码无解（N7 判词）。
// 修复路径 = 本 TU 路由到 C6446R（cc1plus 4.4.6；P230 补：4.4.6 间接 call 目标 = %edx）。
// 实测（同源码仅换后端）：ServerXml.cpp identical 69 / ae 88 / near 4 / diff 1
//   →  identical 69 / ae 92 / near 0 / diff 1（本件 NEAR→AE）。
// 路由表 dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落 *ServerCommon/ServerXml.cpp 路由。
// 注：p167 另有 3 条 C++0x 右值实例调用（_ZNSt4pairIiiEC1IRiS2_EEOT_OT0_ 等）在 ORIG 侧；
//     本件 .o 侧无该实例名 ⇒ 属 P93/模板发射口径，非语义缺口（N130 族）。
// ============================================================================
#include "ServerXml/RGBALoad.inc"

// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族），本件 2 处：
//   ORIG 0x080c6354 / 0x080c63ad: call *%edx（前置 mov (%eax),%edx）
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax
// 助记符序列与指令数全等（83/83），仅寄存器角色由后端决定 ⇒ 源码无解（N7 判词）。
// 修复路径 = 本 TU 路由到 C6446R（cc1plus 4.4.6；P230 补：4.4.6 间接 call 目标 = %edx）。
// 实测（同源码仅换后端）：ServerXml.cpp identical 69 / ae 88 / near 4 / diff 1
//   →  identical 69 / ae 92 / near 0 / diff 1（本件 NEAR→AE）。
// 路由表 dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落 *ServerCommon/ServerXml.cpp 路由。
// ============================================================================
#include "ServerXml/ProcessLoad.inc"

#include "ServerXml/StrPunish.inc"

#include "ServerXml/GetServerString.inc"

#include "ServerXml/GetEventRGBA.inc"

#include "ServerXml/GetEventString.inc"

} // namespace np_server_xml
