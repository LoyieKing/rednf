// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_DevMgr_Server_Proxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CNetwork<6144,100000> 在本 TU 隐式实例化（ORIG 同：10 个 W 符号位于本 TU
// 0x0818df60-0x0818e837；CNetwork.h 的 extern template 已按 Main 批准移除该实例）；
// CNetwork.cpp 保留显式实例化（weak 合并）。TU 内还含 <iostream> 的
// std::ios_base::Init 静态初始化（ORIG _GLOBAL__I_ keyed to C2）。
// 实例化 CNetwork<6144,100000>（显式实例化在 CNetwork.cpp），TU 内还含
// <iostream> 的 std::ios_base::Init 静态初始化（ORIG _GLOBAL__I_ keyed to C2）。
#include <iostream>

// [b28d-28] DIFF | ORIG 0x0818e2c8 | CNetwork<6144,100000>::Parsing_ServerPacket(int)
// （定义在共享头 CNetwork.h，本 TU 隐式实例化）：残差仅 1 条 `jmp` 位置互换
// （P223 边界：4.4 对单源 goto/空 then 尾岛恒 jump-threading 合并，N 封顶）。

#include "Arad_DevMgr_Server_Proxy.h"
#include "LogManager.h"

#include "Arad_DevMgr_Server_Proxy/Arad_DevMgr_Server_Proxy.inc"


#include "Arad_DevMgr_Server_Proxy/open.inc"


#include "Arad_DevMgr_Server_Proxy/init.inc"


#include "Arad_DevMgr_Server_Proxy/disconnect.inc"


#include "Arad_DevMgr_Server_Proxy/recv.inc"


#include "Arad_DevMgr_Server_Proxy/send.inc"

