// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemLimitEdition/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CItemLimitEdition / CItemLimitEditionMgr 实现。
// 唯一权威声明见 CItemLimitEdition.h。逐符号按 ORIG objdump 转写。
//
// ORIG 关键事实：
//   - CItemLimitEdition ctor 0x084e9732：rep movsl ×0x12 复制前 0x48 字节，
//     再把 info+0x18 写 this+0x48（初始 curSellNum）。对象尺寸 0x4c。
//   - makeInfoPacket 0x085130e8 按 18 字段顺序写（含剩余时间 = sellEndTime-now 或 0）。
//   - CItemLimitEditionMgr 用 __gnu_cxx::hash_map；getItemInfo 返回指针，未命中返回 0；
//     updateItem 命中 updateCurSellNum 后返回 1，未命中返回 0（Ghidra：0x084e9904）。
//   - makeItemLimitEditionInfo/RemoveInfo：循环遍历 m_items，写计数与包。

#include "CItemLimitEdition.h"

#include <string.h>

#include "CSystemTime.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"   // PacketGuard 完整定义（本 TU 直接以 packet.<member>() 调用，
                           // 不再经 InterfacePacketBuf* 基类中转 —— ORIG 直读形参形）

// ============================================================================
// CItemLimitEdition
// ============================================================================
#include "CItemLimitEdition/CItemLimitEdition.inc"
}

#include "CItemLimitEdition/dtor_CItemLimitEdition.inc"
}

#include "CItemLimitEdition/updateCurSellNum.inc"
}

// [b28d-51] 判词 N（ORIG 0x085130e8，N82/N86 族）：PacketGuard 引用形参 +
// sret/临时槽方向差；语义/调用集齐，byte 层封顶。
// [round5 已修 IDENTICAL/AE] 真因不是槽方向而是**字段类型**：ORIG 各 put_* 实参装载形
//   两段 `mov mem,%eax; mov %eax,%edx`（与 put_int(int) 异型）vs 单段 `mov mem,%edx`
//   （同型）。探针 /tmp/probe_ci3.cpp（int→int 单段；unsigned→int 两段；enum→int 两段；
//   long→int 单段）+ /tmp/probe_ci4.cpp（unsigned int→unsigned long 单段、int→ulong 两段）
//   定出：8 个字段实为 unsigned int（+0x00/+0x08/+0x10/+0x14/+0x20/+0x2c/+0x30/+0x48）、
//   m_itemIdx 亦 unsigned（put_item_idx 实参单段）、m_avatarTypeSelect 为 signed char
//   （ORIG `movzbl 0xc(%eax),%eax; movsbl %al,%edx`）。头已同步（P254 档③，sizeof/偏移不变）。
//   同时把时间比较按 ORIG 书写方向改回 `m_sellEndTime <= getCurSec()`（ORIG 先载字段到 %ebx
//   跨 call 保活、`cmp %eax,%ebx; setle`）。修后 144=144 AE，getIPGNO 返回类型同步 unsigned。
// [L13-group6 P0/P183] ORIG 每个调用点 fresh 读 `0xc(%ebp)`（`mov 0xc(%ebp),%eax`
// 直压实参），我方具名 `InterfacePacketBuf* buf` 占 -0xc 槽且每处 reload（帧
// 0x24 vs ORIG 0x14）⇒ 删具名局部，以宏（P49 寻址助手用宏）逐点内联同一表达式。
#define CILE_PACKET_BUF(p) ((InterfacePacketBuf*)(void*)&(p))
#include "CItemLimitEdition/makeInfoPacket.inc"
}
#undef CILE_PACKET_BUF

// ---- getters（ORIG 逐字段偏移）----
#include "CItemLimitEdition/getIPGNO.inc"
}

#include "CItemLimitEdition/getRelateditem.inc"
}

#include "CItemLimitEdition/getRelatedItemCnt.inc"
}

#include "CItemLimitEdition/getAvatarTypeSelect.inc"
}

#include "CItemLimitEdition/getPriceCera.inc"
}

#include "CItemLimitEdition/getPriceGold.inc"
}

#include "CItemLimitEdition/getPriceWinPoint.inc"
}

#include "CItemLimitEdition/getTotalSellNum.inc"
}

#include "CItemLimitEdition/isSellComplete.inc"
}

#include "CItemLimitEdition/getCurSellNum.inc"
}

#include "CItemLimitEdition/getSellEndTime.inc"
}

#include "CItemLimitEdition/getFeaturedIdx.inc"
}

#include "CItemLimitEdition/getCondCharacJob.inc"
}

#include "CItemLimitEdition/getCondCharacLevelBegin.inc"
}

#include "CItemLimitEdition/getCondCharacLevelEnd.inc"
}

#include "CItemLimitEdition/getCondCharacCreateTimeBegin.inc"
}

#include "CItemLimitEdition/getCondCharacCreateTimeEnd.inc"
}

#include "CItemLimitEdition/getCondAccCreateTimeBegin.inc"
}

#include "CItemLimitEdition/getCondAccCreateTimeEnd.inc"
}

// ============================================================================
// CItemLimitEditionMgr
// ============================================================================
#include "CItemLimitEdition/CItemLimitEditionMgr.inc"
}

#include "CItemLimitEdition/dtor_CItemLimitEditionMgr.inc"
}

#include "CItemLimitEdition/registItem.inc"
}

#include "CItemLimitEdition/removeItem.inc"
}

#include "CItemLimitEdition/updateItem.inc"
}

#include "CItemLimitEdition/clear.inc"
}

#include "CItemLimitEdition/getItemInfo.inc"
}

#include "CItemLimitEdition/empty.inc"
}

#include "CItemLimitEdition/makeItemLimitEditionInfo.inc"
}

#include "CItemLimitEdition/makeItemLimitEditionRemoveInfo.inc"
}
