// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPvPLiveEventData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPvPLiveEventData::IncreasePlayCount（ORIG 0x826921c）。
// 其余方法在本 TU 复用真实成员布局；loadData 只恢复缓冲区首 dword。
#include "CPvPLiveEventData.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

#include <string.h>
#include <string>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "Inven_Item.h"
#include "ServerParameterScript.h"
#include "RDARScriptStringManager.h"
#include "CGameManager.h"      // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail

extern CDataManager* G_CDataManager();

#include "CPvPLiveEventData/CPvPLiveEventData.inc"
}

// [b28d-20] ORIG 0x0826921c 逐条对齐：①无 dm/字符串指针/group/characNo 具名局部（P0：
// G_CDataManager() 每消费点重发；findString 结果直传 strncpy 实参）；②计数比较用正向包裹
// `if (m_playCount == GetPlayCount()) { 主体 }` 复现 ORIG sete+je 物化（负形 setne+jne 反极性）；
// ③迭代器解引用写 `(*it).first/.second`（ORIG 两处 deEv，非 ptEv）；④find_item 结果具名
// CItem* found（ORIG -0xc 槽往返）；⑤Inven_Item::m_addInfo 写 itemIdx（ORIG mov %eax,-0x6c(%ebp)）；
// ⑥奖励判定正向包裹 `if (item.m_addInfo != 0 || item.get_add_info() != 0)` 复现 ORIG
// jne settrue / je false 的 OR 物化；⑦strncpy 后无显式 '\0'（ORIG 无该写）。
// [b28d-solo-CPvPLiveEventData] verdict=IDENTICAL/AE（ORIG 160 = OURS 160，逐条助记符全等，
// p167 调用点差集 CLEAR）；本波修法：itemIdx/addInfo 由 int 改 unsigned int（P290 实参升格
// callee-saved 跨 G_CDataManager() 保活），复现 ORIG 0x082692c4/0x082692f6 的 %ebx 预装载。
#include "CPvPLiveEventData/IncreasePlayCount.inc"
}

#include "CPvPLiveEventData/loadData.inc"
}
#include "CPvPLiveEventData/_reset.inc"
}

#include "CPvPLiveEventData/ResetDailyMidnight.inc"
}

#include "CPvPLiveEventData/getData.inc"
}

// [b28d-20] ORIG 0x082690d4：每处 operator<< 前调 CStreamGuard::operator*()（0x080c8c46）
// 并把返回 %eax 直接作 operator<< 的 this ⇒ 源形为 `guard.operator*()->operator<<(x)`
// （operator* 返回 CStreamGuard*，指针即 receiver）。实参跨 operator* 调用保活 ⇒ 4.4 自然
// 升格 %ebx（与 ORIG 的 mov %eax,%ebx 形一致）。
// [b28d-solo-CPvPLiveEventData] verdict=IDENTICAL/AE（ORIG 84 = OURS 84，逐条助记符全等，
// p167 调用点差集 CLEAR）；本波修法：GetUID() 调用点 (int) 收窄修重载选择（P3，
// ORIG 0x08269131 选 _ZN12CStreamGuardlsEi，原先误选 lsEj）。
#include "CPvPLiveEventData/_saveData.inc"
}
