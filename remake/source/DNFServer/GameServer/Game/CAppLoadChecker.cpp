// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CAppLoadChecker/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r Game/ CAppLoadChecker 实现（ORIG ServerLoadChecker.cpp）。
// 逐函数 objdump 转写：
//   CAppLoadChecker()        0x85ffc6a (0x48)
//   CAppLoadCheckerInstance  0x85ffc24 (0x46)  guard 0x9473200 / 实例 0x9473208
//   setQueue                 0x86004b0 (0x28)
//   CheckLoad                0x86004d8 (0xa7)
//   RequestDB                0x8600580 (0x12c)
//   checkLoad_dbq            0x85ffcb2 (0x1fd)  基准 +0x00 状态 +0x10
//   checkLoad_logdbq         0x85ffeb0 (0x1ff)  基准 +0x04 状态 +0x11
//   checkLoad_timerq         0x86000b0 (0x1ff)  基准 +0x08 状态 +0x12
//   checkLoad_packetq        0x86002b0 (0x1ff)  基准 +0x0c 状态 +0x13
// 四个 checkLoad_* 已逐一反汇编核对：阈值阶梯完全一致（上行 10/25/40/80/150/250，
// 下行同值），仅基准字段与状态字节不同；按 ORIG 各自独立展开，不共享辅助函数。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit + 把键名符号
// （CAppLoadCheckerInstance，ORIG 0x86006ec）定义前移为本 TU 首强符号。
#include <iostream>

#include "CAppLoadChecker.h"

#include "CEnvironment.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"

namespace {

// SIG_INSERT_SERVER_LOAD_STATISTIC 视图（SigTypes.h 权威类型为 m_pad[0x100]
// 占位）。ORIG RequestDB 写入布局：+0/+1 byte、+2/+4/+6/+8 short。
struct SigInsertServerLoadStatisticView
{
    unsigned char m_serverGroup;  // +0x00
    unsigned char m_channelNo;    // +0x01
    short m_dbq;                  // +0x02
    short m_logdbq;               // +0x04
    short m_timerq;               // +0x06
    short m_packetq;              // +0x08
};

}  // namespace

// [GI-ioinit L21] 本 TU 首个强外部符号定义（_GLOBAL__I_ 键名来源，P327）。
CAppLoadChecker* CAppLoadCheckerInstance()
{
    static CAppLoadChecker instance;   // ORIG _ZZ23...E8instance @0x9473208
    return &instance;
}

#include "CAppLoadChecker/CAppLoadChecker.inc"
}

#include "CAppLoadChecker/setQueue.inc"
}

#include "CAppLoadChecker/checkLoad_dbq.inc"
}

#include "CAppLoadChecker/checkLoad_logdbq.inc"
}

#include "CAppLoadChecker/checkLoad_timerq.inc"
}

#include "CAppLoadChecker/checkLoad_packetq.inc"
}

#include "CAppLoadChecker/CheckLoad.inc"
}

#include "CAppLoadChecker/RequestDB.inc"
}

