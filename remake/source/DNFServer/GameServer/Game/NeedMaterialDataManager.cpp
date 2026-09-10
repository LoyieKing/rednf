// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NeedMaterialDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// 迁移自 GameStubs.cpp NeedMaterialDataManager 桩（batch-6）。
// getInstance / getNeedMaterial(CItem*) 语义按 ORIG 反汇编转写
// （见 NeedMaterialDataManager.h 布局注释）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN23NeedMaterialDataManagerC2Ev`（ORIG 0x81b5c5d）；ctor 已是首强符号。
#include <iostream>


#include "NeedMaterialDataManager.h"
#include "CItem.h"

// [b28c-31] N76：ORIG 0x081b5a4f 先 `lea 0x18(%eax),%edx` 再物化实参（LHS 地址
// 先载），4.4 恒 RHS 先；显式列全成员初始化列表无效 ⇒ 封顶。
// [b28d-11b] N75/P75：this+0x18 地址与实参常量物化序跨版本不可翻（4 形探针：4.4 恒先物化 -1/0 再 lea）；残余 4 条顺序差 @0x081b5a3c
#include "NeedMaterialDataManager/NeedMaterialDataManager.inc"
}

#include "NeedMaterialDataManager/dtor_NeedMaterialDataManager.inc"
}
#include "NeedMaterialDataManager/getInstance.inc"
}

#include "NeedMaterialDataManager/getNeedMaterial.inc"
}
// ORIG _ZN23NeedMaterialDataManager18setNeedMaterialInfoEiRKSt4pairIiiE @ 0x081b5b46
// m_map[idx]（operator[] 默认构造）后整体赋值 pair。
#include "NeedMaterialDataManager/setNeedMaterialInfo.inc"
}
