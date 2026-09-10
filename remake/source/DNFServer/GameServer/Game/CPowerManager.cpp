// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPowerManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <cstring>
#include <cstdio>
#include "CPowerManager.h"
#include "SigTypes.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CGameManager.h"

#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CPrivateStoreMgr.h"
#include "CSharedServerMessageManager.h"
// [b28d-solo] SIG_LOAD_POWER_WAR_STATUE_INFO（0x80）载荷具名视图（P80/P99）：
// 偏移逐字段依 ORIG SetPowerWarUserStatueInfo 反汇编（0x0847ed4e 段）实证；
// +0x4d..+0x4f 为 ORIG 未访问空洞（显式 pad 表达布局，不参与写入）。
#pragma pack(push, 1)
struct PowerWarStatueView
{
    unsigned int m_characNo;   // +0x00
    char m_name[0x1e];         // +0x04
    char m_job;                // +0x22（与目标 m_rankings[].m_job 同型，P211 档③）
    char m_level;              // +0x23
    char m_grow;               // +0x24
    char m_guildName[0x28];    // +0x25
    char m_pad4d[3];           // +0x4d..+0x4f
    int m_guildId;             // +0x50
    int m_equipment[11];       // +0x54（0x2c）
};
#pragma pack(pop)
// ---- CPowerManager（PowerWar 管理器，ORIG 0x0847e998 域，布局 0x208）----
// report: docs/class_func_reports/CPowerManager.md。对象为 GlobalData.cpp
// 的 m_pad[0x208]（offset 布局一致：+0x00 winnerSide / +0x04/+0x08 分数 /
// +0x0c/+0x10 频道分数 / +0x14/+0x18 阵营人数 / +0x28..+0x1b4 雕像排行 /
// +0x1b4 CPowerWarLog / +0x1f0 CPowerWarPacketLog）。
#include "CPowerManager/CPowerManager.inc"
}

#include "CPowerManager/IsPowerWarEventOn.inc"
}



#include "CPowerManager/GetPowerWarResponPenalty.inc"
}

// [b28d-solo-CPowerManager] NEAR(18=18, gap 0) | ORIG 0x08480e92 | 语义全等
// （fildl/fmuls/fistpl 同序、指令数一致），残差仅槽号：ghostTime -0x10、cw
// -0x1a/-0x1c vs ORIG -0x1c/-0x1e/-0x20 ⇒ N82/P112 封顶。本轮穷举六族：
// double framePad 前置/后置、int[3]/char pad、3×dead int、long double、union
// {struct{int;int[3];};int;} 标量视图形、n-first 声明序——4.4 把未使用局部沉到
// 帧底、使用的标量按声明序反向分配，源槽恒 -0x10/-0xc，无法移到 ORIG 的 -0x1c
// （4.1 为不复用临时槽预留 12B 死槽；视图形虽命中 -0x1c 但 fildl 前多一次 spill）。
#include "CPowerManager/GetPowerWarGhostTime.inc"
}


#include "CPowerManager/IncreasePowerUserCount.inc"
}

#include "CPowerManager/DecreasePowerUserCount.inc"
}

#include "CPowerManager/CheckPowerWarEnterLimit.inc"
}

#include "CPowerManager/GetWinnerSide.inc"
}

#include "CPowerManager/IsWinerSide.inc"
}

// [b28d-solo-CPowerManager] AE | ORIG 0x0847fb02 | 388=388 clean 全等，P167 69/69。
// 本轮修复：P0/P23 lastPlay 内联、P0/P52 三值内联实参（%edi/%esi/%ebx 跨 call
// 保活）、P292 档② isOriginal char→bool、P225 while 形复现 enterLimit setne 物化、
// P198① cooldown 两句式预清。
#include "CPowerManager/ProcessJoinPowerWar.inc"
}

#include "CPowerManager/Init.inc"
}

#include "CPowerManager/LoadPowerWarInfo.inc"
}

#include "CPowerManager/LoadRankerInfo.inc"
}
// [b28d-solo-CPowerManager] IDENTICAL(28=28, ndiff 0) | ORIG 0x0847f1b0 | 本轮修复：
// P310 翻案（旧 N18b/P209附 判词作废）——循环局部由 unsigned 改 int + 显式
// `(unsigned int)i <= 2` 比较，4.4 即发 ORIG 的两段寄存器尾测
// `mov slot,%eax; cmp $2,%eax; setbe; test %al,%al; jne`（unsigned 局部恒折
// `cmpl $2,slot` 单段）。同族三件（GetRankingByCharacNo/SetPowerWarUserRankingInfo）
// 同法一次归零。
#include "CPowerManager/CheckCompleteDBLoadStatueInfo.inc"
}
#include "CPowerManager/SetPowerInfo.inc"
}

#include "CPowerManager/SetPowerWarStartTime.inc"
}

// [b28d-solo-CPowerManager] IDENTICAL(104=104, ndiff 0) | ORIG 0x0847ebec | 本轮修复：
// P310 同法（int i + `(unsigned int)i < 3`）复现 ORIG 0x0847ec0e 尾测两段物化
// `mov -0xc(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`；P167 CLEAR 12/12。
#include "CPowerManager/SetPowerWarUserRankingInfo.inc"
}



// [b28d-solo-CPowerManager] 语义完成 / byte 层 N（352=352, gap 0）| ORIG 0x0847ed4e |
// 此前修复：真语义缺口 m_flag=1（ORIG memcpy 后 `movb $0x1,0x8(%eax)`）+ pack(1)
// 具名视图消 add 链（P80/P99/P183）+ P222 正向 if 包裹 + P227/P139 全表达式临时链
// + __PRETTY_FUNCTION__ + P211 档③ 同型化。本轮：P310 消 N18b 尾测 1 条。
// 残差 = N4/N86b（13 处 4.1 `lea 0x0(,%eax,4),%edx` vs 4.4 `shl $0x2,%eax` 强度
// 归约形；探针 /tmp/probe2d{,2}.cpp 八形——一维/二维[3][1]/`*(arr+i)`/
// `(&arr[0])[i]`/指针局部/unsigned 索引/int 数组下标——4.4 恒 shl，源形不可择）。
// P167 CLEAR 7/7。
#include "CPowerManager/SetPowerWarUserStatueInfo.inc"
}

// [b28d-solo-CPowerManager] IDENTICAL(186=186, ndiff 0) | ORIG 0x0847f5de | clean
// 全等（P227 全表达式临时链复现两 trace 槽序 + varargs 跨 ctor 保活）；P139 恢复
// `_ZZN13CPowerManager12ClearMVPInfoEvE19__PRETTY_FUNCTION__` 局部静态符号。
#include "CPowerManager/ClearMVPInfo.inc"
}

#include "CPowerManager/IsAbleToRevival.inc"
}

#include "CPowerManager/IncreaePowerWarPointInChannel.inc"
}

// [b28d-solo-CPowerManager] IDENTICAL(237=237, ndiff 0) | ORIG 0x0847f200 | 本轮修复：
// P310（循环变量 i 由 unsigned 改 int + 显式 `(unsigned int)i < 3`）复现 ORIG 尾测
// 两段物化 `mov -0x14(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`，旧「N18b 尾测
// 1 条」判词作废。此前：len 具名缓存消 EXTRA +2 strlen（P167 22/22 CLEAR）、
// P47 正极性 if/else、P212 全头部声明（槽位全等 pkt-0x28/count-0x1c/len-0x18/
// i-0x14/i2-0x10/j-0xc）、P171 死预清 len=0。
#include "CPowerManager/SendPowerWarUserStatueInfo.inc"
}

#include "CPowerManager/GetPowerUserCount.inc"
}

#include "CPowerManager/GetPowerWarPoint.inc"
}

#include "CPowerManager/GetPowerWarPointInChannel.inc"
}

// [b28d-solo-CPowerManager] IDENTICAL(28=28, ndiff 0) | ORIG 0x0847eba0 | 本轮修复：
// P310 翻案（旧 N18b/P209附 判词作废）——int 局部 + 显式 `(unsigned int)i <= 2`
// 复现 ORIG 0x0847ebd8 尾测 `mov slot,%eax; cmp $2,%eax; setbe; test; jne`。
#include "CPowerManager/GetRankingByCharacNo.inc"
}

// ORIG 0x084ebe2e（_ZN14CPowerManager16SetPowerWarPointEii，W）：
//   *(int*)(this+0x04) = arg1; *(int*)(this+0x08) = arg2; ret。
#include "CPowerManager/SetPowerWarPoint.inc"
}
