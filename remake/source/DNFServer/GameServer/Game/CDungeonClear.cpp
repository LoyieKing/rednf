// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDungeonClear/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CUserCharacInfo.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "QuestClear.h"  // WongWork::CQuestClear 唯一权威定义
#include "WongWorkEvents_A.h"  // WongWork::arad_parsing（ORIG 0x08483e98）
#include "OS_API.h"

#include <boost/tokenizer.hpp>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

namespace WongWork
{

// ----------------------------------------------------------------------------
// arad_parsing —— ORIG T 0x08483e98 (size 0x115)
// b28d-60 跨 TU 迁移（P210②，Main 批准）：ORIG TU = DungeonClear.cpp
//（orig_tu_names.tsv:429 GI 键名 `_GLOBAL__I__ZN8WongWork12arad_parsing...`；
//  且 ORIG 中本函数 0x08483e98 结束 0x08483fad 紧接
//  CDungeonClear::AradfromString T 0x08483fae）。原定义在 WongWorkEvents_A.cpp。
// start=find_first_not_of(delims,pos)/end=find_first_of(...)，非全 npos 则循环
// substr 入 vector（move push_back）；尾段 end==npos 但 start!=npos 也输出。
// ----------------------------------------------------------------------------
#include "CDungeonClear/arad_parsing.inc"
}

#include "CDungeonClear/AradfromString.inc"
}

#include "CDungeonClear/enumDungeonClear.inc"
}

#include "CDungeonClear/fromString.inc"
}

#include "CDungeonClear/toString.inc"
}

#include "CDungeonClear/toString_1.inc"
}

} // namespace WongWork
