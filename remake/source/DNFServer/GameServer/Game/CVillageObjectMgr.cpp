// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CVillageObjectMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CVillageObjectMgr.cpp
#include "CVillageObjectMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PacketGuard.h"
#include "PacketBuf.h"   // put_header/get_index/put_byte/finalize

namespace village_object
{

// ORIG 0x86b6f30：仅默认构造成员 std::map<Zone, list<IObject*>> m_objects。
#include "CVillageObjectMgr/CVillageObjectMgr.inc"
}

#include "CVillageObjectMgr/Zone.inc"
}

// ORIG 0x86b6f44：调用 std::map<...>::~map（成员自动析构）。
#include "CVillageObjectMgr/dtor_CVillageObjectMgr.inc"
}

#include "CVillageObjectMgr/on_move_area.inc"
}

#include "CVillageObjectMgr/register_object.inc"
}

}  // namespace village_object
