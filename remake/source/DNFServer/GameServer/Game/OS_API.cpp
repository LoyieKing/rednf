// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OS_API/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "OS_API.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "CMyFileLog.h"
char* NumberToString(unsigned int value, int radix);   // 定义 LinkFinalDeps.cpp/ServerCommon

// 5 个 ORIG 符号的实现（df_game_r 0x0858c53a..0x0858c789）。
// 其余 5 符号（GetDateTimeTick/snprintf/Sleep/WriteLog/uSleep）由既有文件以
// namespace OS_API 定义，声明见 OS_API.h 与 CInventory.h。
// 全部为自由函数（ORIG mangled 与 namespace 函数一致，均无 this 参与）。

namespace OS_API
{

// ORIG 0x0858c53a _ZN6OS_API4ExitEi：直接 exit。
#include "OS_API/Exit.inc"
}

// ORIG 0x0858c6d4 _ZN6OS_API17WriteLastBookmarkEPKci：
// fopen(path,"wb") 成功则 fprintf "bm:%d" 后 fclose。
#include "OS_API/WriteLastBookmark.inc"
}

// ORIG 0x0858c54c _ZN6OS_API12WriteLastSigEPKcS1_iiPKvi：
// 256 栈缓冲清零 → sprintf "./log/%s/%s" → fopen("wb") →
// fprintf "K:%d C:%d P:%p SZ:%d\n" → fclose。
#include "OS_API/WriteLastSig.inc"
}

// ORIG 0x0858c5f4 _ZN6OS_API12WriteLastSigEPKcS1_jiiPKviiiiii：
// 512 栈缓冲清零 → sprintf "./log/%s/%s" → NumberToString(u,0) →
// CMyFileLog(__PRETTY_FUNCTION__, 0x4e) → log(path, "U:%s ...", us, 后续 9 参)。
#include "OS_API/WriteLastSig_1.inc"
}

// ORIG 0x0858c71e _ZN6OS_API14WriteLoginInfoEPKcS1_z：
// fopen(path,"wb") 成功则 vsprintf 到 1024 栈缓冲 → fputs → fclose。
#include "OS_API/WriteLoginInfo.inc"
}

}  // namespace OS_API
