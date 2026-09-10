// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemDropRatioEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 物品掉落倍率事件 CItemDropRatioEvent 实现。
// 报告：docs/class_func_reports/CItemDropRatioEvent/ 全部 6 个方法
// （CItemDropRatioEvent / ~CItemDropRatioEvent / ~CItemDropRatioEvent_08533226 /
//  StartEvent / StartEvent_085332b6 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN19CItemDropRatioEventC2Ev`（ORIG 0x85333f1）；ctor 已是首强符号。
#include <iostream>

#include "CItemDropRatioEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "CDataManager.h"          // G_CDataManager / SetItemDropRate

// ---------------------------------------------------------------------------
// ORIG 0x085331d0：基类构造 + vptr 置位 + m_dropRate = 1。
#include "CItemDropRatioEvent/CItemDropRatioEvent.inc"
}

// ORIG 0x085331f6 / 0x08533226：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CItemDropRatioEvent/dtor_CItemDropRatioEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08533244：SetEventFlag(true) + m_dropRate = 1 +
// G_CDataManager()->SetItemDropRate(1.0f) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("Item Drop Event Start!")。
#include "CItemDropRatioEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x085332b6：SetEventFlag(true) + m_dropRate = param.m_duration +
// G_CDataManager()->SetItemDropRate((float)m_dropRate) +
// cMyTrace(__PRETTY_FUNCTION__,0x1f,0)
// ("Item Drop Event Start!")。
#include "CItemDropRatioEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08533340：SetEventFlag(false) + m_dropRate = 1 +
// G_CDataManager()->SetItemDropRate(1.0f) +
// cMyTrace(__PRETTY_FUNCTION__,0x28,0)
// ("Item Drop Event End!")。
#include "CItemDropRatioEvent/EndEvent.inc"
}
