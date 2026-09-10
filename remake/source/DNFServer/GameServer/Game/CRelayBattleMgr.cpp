// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CRelayBattleMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CRelayBattleMgr（G2-4 PvP 链，接力战管理器，PvP_Room +0x620）还原。
// 逐函数对照 docs/class_func_reports/CRelayBattleMgr.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// PvP_Room / CRelayBattle / CRandomValue / CDescending 跨类方法按声明引用
// ORIG 真实符号（不新造桩；链接桩由 PvP_deps.cpp / 主 agent 提供）。
#include <algorithm>
#include <cstring>
#include <vector>

#include "CRelayBattleMgr.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "PvP_Room.h"
#include "TimerQueue.h"
#include "PacketGuard.h"

// ============================================================================
// GlobalData（CUserCharacInfo.h 已声明 namespace GlobalData；补缺失成员）
// ============================================================================
namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ============================================================================
// 依赖子对象 / 外部函数（PvP_Room.h / TimerQueue.h 权威头提供真实声明）
// ============================================================================

// ---- 随机排序辅助（ORIG 弱符号，定义归 PvP_deps.cpp / 主 agent） ----
class CRandomValue
{
public:
    CRandomValue(std::vector<int>& seq, int a, int b, int c);
    int operator()();

    std::vector<int>* m_pSeq;  // +0x00
    int m_end;                 // +0x04
    int m_start;               // +0x08
    int m_count;               // +0x0c
};

class CDescending
{
public:
    bool operator()(int a, int b);
};

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义） ----


// ============================================================================
// CRelayBattle（0xc，接力战单槽子对象；ORIG 0x085de01a）
// ============================================================================
#include "CRelayBattleMgr/Reset.inc"
}

// ============================================================================
// 生命周期 / 房间钩子
// ============================================================================
#include "CRelayBattleMgr/Reset_1.inc"
}

#include "CRelayBattleMgr/SetParent.inc"
}

#include "CRelayBattleMgr/OnCreateRoom.inc"
}

#include "CRelayBattleMgr/OnJoinRoom.inc"
}

#include "CRelayBattleMgr/OnLeaveRoom.inc"
}

#include "CRelayBattleMgr/OnDiePlayer.inc"
}

#include "CRelayBattleMgr/OnChangeTeam.inc"
}

#include "CRelayBattleMgr/OnReady.inc"
}

// ORIG 0x085de18c：条件读形 `movzbl 0x7(%eax,%ecx,1),%eax; xor $0x1,%eax;
// test %al,%al; je`。旧判词 N96+N18a（"寄存器物化无绑定不可达"）已作废——
// 正解 = 字段 m_requestFight **bool 化**（P248 档②）+ 裸 if(!f)，零布局影响。
#include "CRelayBattleMgr/OnRequestFight.inc"
}

#include "CRelayBattleMgr/OnStart.inc"
}

#include "CRelayBattleMgr/OnFight.inc"
}

// ============================================================================
// 访问器
// ============================================================================
#include "CRelayBattleMgr/GetKillCount.inc"
}

#include "CRelayBattleMgr/GetTimerKey.inc"
}

#include "CRelayBattleMgr/GetChangeSequenceCount.inc"
}

#include "CRelayBattleMgr/GetAllKillUserIndex.inc"
}

// ============================================================================
// 顺序编排
// ============================================================================
#include "CRelayBattleMgr/IncreaseChangeSequenceCount.inc"
}

#include "CRelayBattleMgr/DecreaseChangeSequenceCount.inc"
}

#include "CRelayBattleMgr/ResetChangeSequenceCount.inc"
}

#include "CRelayBattleMgr/CreateRandomSequenceVector.inc"
}

#include "CRelayBattleMgr/ArrangeChangeSequence.inc"
}

// [b29 P248档② 实修] ORIG 0x085de83e 条件读形 `movzbl 0x7(%eax,%ecx,1),%eax;
// xor $0x1,%eax; test %al,%al; je` ⇒ m_requestFight bool 化 + 裸 if(!f)。
// 旧判词 N96+N18a（"寄存器物化无绑定不可达"）作废：fresh 改后 DIFF→AE（62=62）。
#include "CRelayBattleMgr/ArrangeNoChangeSequence.inc"
}

#include "CRelayBattleMgr/ArrangeMatchSequence.inc"
}

#include "CRelayBattleMgr/GetNextPlayer.inc"
}

#include "CRelayBattleMgr/SendRequestFight.inc"
}

// [b29-R28 实修] ORIG 0x085dead6：150/150（gap=0）⇒ 本 TU 现 ae（near=0）。
// 修法 = P171 形态复原：`guard.put_byte(0)` → `guard.put_byte(count)`，ORIG
// 0x085deb71/0x085deb74 为「movl $0x0,-0x10(%ebp) → mov -0x10(%ebp),%eax →
// mov %eax,0x4(%esp)」= 读 count 槽而非立即数 0（原写法少 1 条且形态错）。
// 备注（非本件判定项）：OURS 槽 guard=-0x28/index=-0x1c vs ORIG guard=-0x24/
// index=-0x28 系 4.4「聚合优先占低槽、标量分区」分区差（N82附）；删具名 index
// 改内联实参已实测反劣（破坏 ORIG get_index 调用序 + 编译失败）⇒ 保留。
#include "CRelayBattleMgr/TurnPlayer.inc"
}
