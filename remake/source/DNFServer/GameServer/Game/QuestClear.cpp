// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 QuestClear/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "QuestClear.h"
#include <stdarg.h>
#include <string.h>
// P338：ORIG 本 TU 含 std::__ioinit（0x093f57ee，b）与
// _GLOBAL__I__ZN8WongWork11CQuestClear15setClearedQuestEj / __static_initialization_and_destruction_0ii
// （0x0808bc03/0x0808bc43，GI 体 sub $0x18）⇒ 原码含 <iostream>（__ioinit 为 ORIG 实有符号）。
#include <iostream>

namespace WongWork
{

// ORIG 0x0808ba78：TU 内第一个**非内联**函数 ⇒ 决定 _GLOBAL__I_ 键名
// （ORIG 键 = _GLOBAL__I__ZN8WongWork11CQuestClear15setClearedQuestEj）；
// 构造/析构已按 ORIG 弱符号形改头内联定义（见 QuestClear.h）。
#include "QuestClear/setClearedQuest.inc"
}

#include "QuestClear/clear.inc"
}

#include "QuestClear/isClearedQuest.inc"
}

#include "QuestClear/getClearedQuest.inc"
}

#include "QuestClear/resetClearedQuests.inc"
}

#include "QuestClear/setClearedQuest_1.inc"
}

#include "QuestClear/syncScript.inc"
}

#include "QuestClear/checkSaveFlag.inc"
}

#include "QuestClear/getClearedQuestNum.inc"
}

#include "QuestClear/resetSaveFlags.inc"
}

#include "QuestClear/enumQuestClear.inc"
}

}  // namespace WongWork
