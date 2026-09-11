// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFatigueBatteryHandle/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CFatigueBatteryHandle.cpp —— ORIG 报告 docs/class_func_reports/CFatigueBatteryHandle/。
//
// 布局证明（本 TU 内静态断言）：ORIG 全部裸偏移基于 G_CDataManager()+0x68 起点，
// 对应本工程 CDataManager::m_serverParameter(+0x68)；表内 +0x5c0 即
// ServerParameterScript::m_fatigueTable5c0，绝对偏移 0x68+0x5c0 == 0x628。
// NotEndLevel 的 mgr+0x7cc/0x7ce/0x7d0 落在同一表的 +0x1a4/+0x1a6/+0x1a8。
#include "CFatigueBatteryHandle.h"
#include <stddef.h>  // offsetof（布局断言）
#include <string.h>  // memset（stFatigueBatteryConstantTable_t ctor）

#include "CDataManager.h"
#include "CEnvironment.h"
#include "CStatisticServerProxy.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "PacketHeader.h"
#include "GameTypes.h"

// 布局等价证明：m_serverParameter 必须在 CDataManager+0x68（表成员自身偏移已由
// ServerParameterScript.h 的 static_assert 锁定为 +0x5c0）。
static_assert(offsetof(CDataManager, m_serverParameter) == 0x68,
              "CDataManager::m_serverParameter drift: fatigue battery table must sit at +0x628");

// ---- Packet_Fatigue_Battery_Money_Statistic（仅本 TU 使用）----
// ORIG C1 0x084a84ee：PacketHeader(0xc4a,0x11)；byte+0xa=0、int+0xb=0、short+0xf=0。
class Packet_Fatigue_Battery_Money_Statistic : public PacketHeader
{
public:
    Packet_Fatigue_Battery_Money_Statistic()
        : PacketHeader(0xc4a, 0x11), m_serverGroup(0), m_fieldB(0), m_grownupBuff(0)
    {
    }

    unsigned char m_serverGroup;   // +0xa
    unsigned int m_fieldB;         // +0xb
    unsigned short m_grownupBuff;  // +0xf
} __attribute__((packed));

#include "CFatigueBatteryHandle/CFatigueBatteryHandle.inc"

#include "CFatigueBatteryHandle/stFatigueBatteryConstantTable_t.inc"


#include "CFatigueBatteryHandle/getLimitGrownupBuffCntFatigueBattery.inc"


#include "CFatigueBatteryHandle/ComputeFatigueBatteryCharging.inc"


#include "CFatigueBatteryHandle/ChargingFatigueBattery.inc"


#include "CFatigueBatteryHandle/ActiveFatigueBattery_NotEndLevel.inc"


#include "CFatigueBatteryHandle/ActiveFatigueBattery.inc"


#include "CFatigueBatteryHandle/ActiveFatigueBattery_EndLevel.inc"


#include "CFatigueBatteryHandle/dtor_CFatigueBatteryHandle.inc"
