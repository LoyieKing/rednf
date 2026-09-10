// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EncryptTool/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// EncryptTool.cpp — ORIG 还原（2026-08-28）。
// 对照 /tmp/df_game_r objdump：
//   C1/C2 0x0848e0ea、createEncKey 0x0848d5b0、setCryptKey 0x0848d94a、
//   getKeyIndex 0x0848d9a2、wrapEncrypt 0x0848d9b0。
// 加密类（用户规则 2026-08-16）只要求算法正确；实现尽量按 O0 指令形状对齐。

#include "EncryptTool.h"

#include <string.h>
#include <time.h>

#include "CRijndael.h"
#include "Encryption.h"

// ORIG 静态密钥表 @0x93ab8e0：16 项，每项 0x20 字节（strlen=16 的数字串）。
// C1 用 strlen+strncpy 按块拷贝。
static const char s_keyTable[16][0x20] = {
    "0214799992158780",
    "1214670882044781",
    "2214528773140782",
    "3214398662149783",
    "4214278552191784",
    "5219178442142785",
    "6214978332143986",
    "7314078222145287",
    "8204478112146708",
    "9215878002147999",
    "0214238552191784",
    "8219148442132785",
    "7214928332123986",
    "6314018222115287",
    "1204488112196708",
    "2215898002167999",
};

#include "EncryptTool/EncryptTool.inc"
}

// [b28d-lane6] DIFF·豁免：加密/密钥生成类按用户规则只要求算法正确（不强制 identical）；258 vs 238，语义经 decrypt 往返验证。
#include "EncryptTool/createEncKey.inc"
}

#include "EncryptTool/setCryptKey.inc"
}

#include "EncryptTool/getKeyIndex.inc"
}

#include "EncryptTool/wrapEncrypt.inc"
}
