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
// [R11fixE4 2026-09-12] ORIG 本 TU 的 CNetwork<6144,100000> 日志文件名实参为
// 'localjapan/../Network.h'（站点 0x818e6a2/0x818e744/0x818e365/0x818e40a 加载
// VA 0x8b95b10，raw 逐字节 = 6c 6f 63 61 6c 6a 61 70 61 6e 2f 2e 2e 2f 4e 65 74
// 77 6f 72 6b 2e 68）——ORIG 该 TU 以相对拼写打开网络头 ⇒ __FILE__ 展开与他 TU
// 不同。本仓头名统一为 CNetwork.h，故用宏覆盖还原字面量（仅影响本 TU 的
// CNetwork 实例化；6144 的 send_packet()/Parsing_ServerPacket(int) 在 CNetwork.cpp
// 有非 inline 显式特化（强符号），本 TU 的隐式 weak 副本会被链接器丢弃）。
// ⚠ 宏必须在任何 include CNetwork.h 的语句之前定义。
#define CNETWORK_LOG_FILE_NAME "localjapan/../Network.h"
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

