// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameSecurity/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GameSecurity.cpp —— 包安全自由函数实现（PacketDispatcher 叶子批次）。
// 逐指令转写自 ORIG df_game_r（objdump -d）：
//   DenyPacket(int)                          ORIG T 0x859092a (0x104)
//   _NS_PI_Check_CMD(...)                    ORIG T 0x8274778 (0x1fd)
//   _NS_PI_2ND_MakeChecksumTo1Byte2(...)     ORIG T 0x808cc4b (0x1f)

#include "PacketSecurity.h"

#include "PacketBuf.h"
#include "CUser.h"      // WongWork::ENUM_HACKTYPE / CHackAnalyzer / SetETC / 组队判定
#include "CParty.h"     // CParty::get_member_count（组队误判豁免）
#include "md5.h"        // md5_context/md5_starts/md5_update/md5_finish（_NS_MakeHash_）

#include <map>
#include <utility>
#include <time.h>
#include <string.h>

#include "GameSecurity/_NS_MakeHash_.inc"
#include "GameSecurity/PI_GetType_CS.inc"
#include "GameSecurity/PI_GetHackType.inc"
#include "GameSecurity/PIAddServerHackCnt.inc"
#include "GameSecurity/_NS_PI_2ND_GetHackType.inc"


#include "GameSecurity/PI_GetType_CS_1.inc"


#include "GameSecurity/DenyPacket.inc"


#include "GameSecurity/_NS_PI_2ND_MakeChecksumTo1Byte2.inc"


#include "GameSecurity/_NS_PI_Check_CMD.inc"


#include "GameSecurity/_NS_MakeHash__1.inc"


#include "GameSecurity/_NS_MakeHash__2.inc"


#include "GameSecurity/PI_GetHackType_1.inc"


#include "GameSecurity/PIAddServerHackCnt_1.inc"

#include "GameSecurity/HP_GetHackType.inc"


#include "GameSecurity/HIP_GetHackType.inc"


#include "GameSecurity/MI_GetHackType.inc"


#include "GameSecurity/_NS_IPScan.inc"


#include "GameSecurity/_NS_ProcessScan.inc"


#include "GameSecurity/_NS_PacketIntegrity.inc"


#include "GameSecurity/_NS_MI_RecvMemoryIntegrityModulation.inc"


#include "GameSecurity/_NS_NonClient_GetResponse.inc"


#include "GameSecurity/_NS_PI_2ND_RecvPacketIntegrityModulation_C2C.inc"


#include "GameSecurity/_NS_PI_2ND_RecvPacketIntegrityModulation_S2C.inc"


#include "GameSecurity/checkNewMemberHacklog.inc"


#include "GameSecurity/_NS_PI_2ND_GetHackTypeEnum.inc"

#include "GameSecurity/_NS_PI_2ND_GetHackType_1.inc"


#include "GameSecurity/_NS_PI_2ND_GetHackTypeEnum_1.inc"

#include "GameSecurity/_NS_HS_NoResponseToHackLog.inc"

