// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_EventPeriodDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD::Arad_EventPeriodDataManager 实现
//（批3：从 GameStubs.cpp 迁移；批4：补 Arad_InterfaceGameScript 基类生命周期）。
//
// ORIG ctor 0x81953fa：Arad_InterfaceGameScript("Arad_EventPeriodDataManager") 基类 +
// vector<SEventPeriodData>(+0x0c) / vector(+0x18) / Mutex(+0x24) / list(+0x3c)，+0x08=0。
// Load() 0x8195c9a：清理 + 装载事件周期脚本（loadScriptData）。
// isApplied 0x81956b0：在 +0x0c vector 按事件码找周期项，判定 [m_beginTime, m_endTime)。
#include "Arad_EventPeriodDataManager.h"

#include <algorithm>

#include "CUser.h"       // get_acc_id / get_charac_no
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM 权威声明
#include "GlobalData.h"  // s_pGMAccounts_
#include "LogManager.h"
#include "MoreTypes.h"   // arad::SigLoadRewardEventItem 完整定义

namespace ARAD
{

#include "Arad_EventPeriodDataManager/Arad_EventPeriodDataManager.inc"
}

#include "Arad_EventPeriodDataManager/dtor_Arad_EventPeriodDataManager.inc"
}

#include "Arad_EventPeriodDataManager/loadScriptData.inc"
}

#include "Arad_EventPeriodDataManager/clearScriptData.inc"
}

// [b28d-28] DIFF | ORIG 0x081956b0 | N86/P28：find_if sret 临时槽序（ORIG
// -0x1c/-0x18 vs 4.4 -0x18/-0x14）+ 谓词实参装载序；语义等价。
#include "Arad_EventPeriodDataManager/isApplied.inc"
}

#include "Arad_EventPeriodDataManager/isRunning.inc"
}

// ORIG 0x08195994：遍历 SigLoadRewardEventItem 条目（count=+0x04 short），
// 每条以 {事件码@+0x06+4i, accId, 角色号@+0x2e+4i} 构造 EventRewardItemArad；
// 未奖励过则 addUserRewardData 记录，否则 logFormat 打印 already reward。
#include "Arad_EventPeriodDataManager/addUserRewardData.inc"
}

// ORIG 0x08195aa4：Guard<Mutex>(this+0x24) 后 push_back 到 +0x3c list。
#include "Arad_EventPeriodDataManager/addUserRewardData_1.inc"
}

// ORIG 0x081958b6：Guard<Mutex> 后 std::find（operator== 三字段比较），命中即 true。
// [b28d-28] DIFF | ORIG 0x081958b6 | N86/P28：std::find 迭代器槽序 + EH 块序
// （ORIG 两段 return 直落 vs 4.4 jmp 岛）；语义等价。
#include "Arad_EventPeriodDataManager/isRewarded.inc"
}

// ORIG 0x08195794：SearchEventPeriod 谓词 find_if +0x0c vector；
// 命中后起点 = GM ? m_field8 * (-86400) + 0x5005D250 : 0x5005D250（32 位回绕，
// -0x15180 = -86400 秒/天）；begin <= time < 条目 m_endTime(+0x08) 返回 true。
// 注意：ORIG 此函数不加锁。
// [b28d-28] DIFF | ORIG 0x08195794 | N86/P28：同 isApplied（find_if 槽序 +
// 谓词实参序）+ isGM 分支块序；语义等价。
#include "Arad_EventPeriodDataManager/checkCreateTime.inc"
}

// ORIG 0x08195b06：Guard<Mutex> 后删除 +0x3c list 中 +0x04 == get_acc_id(user) 的记录。
// [b28d-28] DIFF | ORIG 0x08195b06 | N86/P28：list erase 迭代器槽序
// （ORIG -0x18/-0x1c vs 4.4 -0x1c/-0x18）+ 比较方向（P220）；语义等价。
#include "Arad_EventPeriodDataManager/delUserRewardData.inc"
}

// ORIG 0x0819589e：param >= 0 时写入 +0x08。
// [b28 BM11] P223：ORIG js→空块 nop 岛 + body 尾 jmp；4.4 对 if{} 恒
// jump-thread（少 jmp+nop），复现形=「负条件 goto 尾空标签」。
#include "Arad_EventPeriodDataManager/setCreatePeriod_GM.inc"
}

}  // namespace ARAD
