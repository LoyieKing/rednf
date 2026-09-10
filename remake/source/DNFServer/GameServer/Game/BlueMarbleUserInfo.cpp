// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BlueMarbleUserInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarbleUserInfo（G2-4 大富翁子对象）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleUserInfo.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// BuffInfo/TileIndexInfo 为跨类依赖；本 TU 直接按 ORIG C1/reset 语义清零字段，
// 不调用 BuffInfo.h（0x10 与 ORIG 0xc 布局分歧）/TileIndexInfo 的 reset 符号
// （避免跨 TU 重复定义与尺寸错配），§9 落地。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN18BlueMarbleUserInfoC2Ev`（ORIG 0x80d9e63）；ctor 已是首强符号定义。
#include <iostream>

#include "BlueMarbleUserInfo.h"

// ============================================================================
// BlueMarbleUserInfo 实现
// ============================================================================

// BlueMarbleScriptManager 最大 buff 值位于 +0x64（ORIG setExp/setGold/setDrop 直读）。
namespace
{
struct BM_ScriptMaxBuff
{
    char m_pad[0x64];
    int m_maxBuffValue;  // +0x64
};
}

// ============================================================================
// BlueMarbleUserInfo 实现
// ============================================================================

// ORIG 0x080d6a90：标量按声明序 init-list 直写 → 成员 C1 call(+0x10/+0x1c)
// → +0x24 直写 → 显式 reset() 两 call（C1 后二次 reset 语义照抄）。
#include "BlueMarbleUserInfo/BlueMarbleUserInfo.inc"
}

#include "BlueMarbleUserInfo/dtor_BlueMarbleUserInfo.inc"
}

#include "BlueMarbleUserInfo/resetBlueMarbleUserInfo.inc"
}
#include "BlueMarbleUserInfo/resetBuff.inc"
}

#include "BlueMarbleUserInfo/resetIndex.inc"
}

#include "BlueMarbleUserInfo/setUser.inc"
}

#include "BlueMarbleUserInfo/getUser.inc"
}

#include "BlueMarbleUserInfo/setScript.inc"
}

#include "BlueMarbleUserInfo/setUserState.inc"
}

#include "BlueMarbleUserInfo/getUserState.inc"
}

#include "BlueMarbleUserInfo/setBoardZone.inc"
}

#include "BlueMarbleUserInfo/getBoardZone.inc"
}

#include "BlueMarbleUserInfo/setGrade.inc"
}

#include "BlueMarbleUserInfo/getGrade.inc"
}

#include "BlueMarbleUserInfo/setDungeon.inc"
}

#include "BlueMarbleUserInfo/getDungeon.inc"
}

#include "BlueMarbleUserInfo/setItem.inc"
}

#include "BlueMarbleUserInfo/getItem.inc"
}

#include "BlueMarbleUserInfo/setExp.inc"
}

#include "BlueMarbleUserInfo/getExp.inc"
}

#include "BlueMarbleUserInfo/setGold.inc"
}

#include "BlueMarbleUserInfo/getGold.inc"
}

#include "BlueMarbleUserInfo/setDrop.inc"
}

#include "BlueMarbleUserInfo/getDrop.inc"
}
