// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CFatigueBatteryHandle/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG 0x84a7ff8 / 0x84a7ffe：均为纯 ret 空体（W 弱符号）。
// （此前 ctor 定义暂置于 GlobalData.cpp，现随本 TU 统一收口。）
#include "CFatigueBatteryHandle/CFatigueBatteryHandle.inc"
#include "CFatigueBatteryHandle/stFatigueBatteryConstantTable_t.inc"
}

// ORIG 0x084a84b6：
//   idx = level-1；idx > (int)m_endLevel10（字节零扩展）→ 返回 0；
//   level <= 0 → 返回 0；
//   否则返回 *(u16*)(&m_field2 + idx + 8)，与 ORIG 的
//   movzwl 0x2(this,idx+8,2) 同址。
#include "CFatigueBatteryHandle/getLimitGrownupBuffCntFatigueBattery.inc"
}

// ORIG 0x84a8004：
//   商 = (charge*level) / 零扩展除数字节（idivl；除数 0 时与 ORIG 同样触发 #DE）；
//   积 = (倍率*specialCnt + 1.0) * 商，全程 x87 80 位中间精度（flds/fild/fmulp 链）；
//   fistpll 以 RC=truncate 向零截断为 64 位，仅低 32 位参与比较与返回（ret 只带 eax）：
#include "CFatigueBatteryHandle/ComputeFatigueBatteryCharging.inc"
}

// ORIG 0x84a8094：
//   level < m_minChargeLevel9 或 level == m_endLevel10 → 不充能返回 0；
//   否则 remain = max(DEFAULT_MAX_FATIGUE-used,0) + DEFAULT_MAX_FATIGUE*v，
//   记录 trace 后返回 Compute(remain, level, specialCnt) 的截 short。
// 注意：used/alreadyFatigue/charNo/level 均按 ORIG 汇编次序读取并进入日志。
#include "CFatigueBatteryHandle/ChargingFatigueBattery.inc"
}

// ORIG 0x84a829e：电池量 >= 表门槛(m_batteryThreshold1a8 == mgr+0x7d0) 且开关
// (m_activeSwitch1a4 == mgr+0x7cc) != 0 时，按倍率计算 grown-up buff 增量、
// 以等级上限截断，经统计代理发送 Packet_Fatigue_Battery_Money_Statistic 并回写；
// 返回本次消耗的电池量（未激活时 0）。
#include "CFatigueBatteryHandle/ActiveFatigueBattery_NotEndLevel.inc"
}

// ORIG 0x84a83da：minLevel <= level 且 level < endLevel 时走 NotEndLevel 分支。
#include "CFatigueBatteryHandle/ActiveFatigueBattery.inc"
}

// ORIG 0x84a81f4：used=(int)(short)GetFatigueBatteryCharging()；result=used；
// 若二次读取与 result 不等则记 trace "FATIGUE BATTERY ACTIVE ERR ..." 并重取。
// ORIG 另有 call G_CDataManager(); add $0x68,%eax 死存储（结果从不读取），按原样保留。
#include "CFatigueBatteryHandle/ActiveFatigueBattery_EndLevel.inc"
}

#include "CFatigueBatteryHandle/dtor_CFatigueBatteryHandle.inc"
