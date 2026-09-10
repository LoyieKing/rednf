// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CFieldLengthChecker/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CFieldLengthChecker.cpp — ORIG 转写（df_game_r elf32-i386）
// AddField 双分支：静态 fieldLength_ 空 → 解析字段定义（'(' ')' ';' 分隔，填充
// 静态 fieldNames_/fieldLength_）；实例成员空 → 解析 "values(" 数据行，按列
// 更新静态 fieldLength_ 为实测最大长度（'...' 引号值去转义、trim）。
#include <iostream>
#include "CFieldLengthChecker.h"
#include "CEnvironment.h"

// [b28d-Lane16] TU 归属迁移（否则 GI 键名不匹配）：
// ORIG 本 TU（= SyncScript.cpp，`orig_tu_names.tsv:107`
// `086269a2 _GLOBAL__I__Z22GetSocketTypeCharactert`）中 `GetSocketTypeCharacter`
// @0x0861ca1b 是首个全局函数声明 ⇒ GI 键名取它；定义必须置于本文件最前。
// ORIG T 0x0861ca1b：socket 类型 → 字符。1→'1' 2→'2' 4→'3' 8→'4'
// 0x10→'5' 0xffef→'6' 0xffff→'7'，其余 0（决策树见 ORIG 反汇编）。
#include "CFieldLengthChecker/GetSocketTypeCharacter.inc"
}

// fieldNames_/fieldLength_ 的权威定义在本 TU（判据 P186/P108 键名 + 地址邻接）：
// ORIG static_init @0x0862690c 依次构造 ioinit@0x94734d6、fieldNames_@0x94734a0、
// fieldLength_@0x94734ac，且与 `CFieldLengthChecker::AddField`@0x086272e8、
// `sync_script::DisplayMaxLength`@0x0861ca94、`GetSocketTypeCharacter`@0x0861ca1b
// 同属一个 TU（GI 键名 `_GLOBAL__I__Z22GetSocketTypeCharactert`）。
// 反向证据：CItemList TU 的 ORIG static_init @0x0851398b 只含 ioinit（26 条），
// 故两个静态定义不得留在 CItemList.cpp（否则本 TU GI 缺 2 对象、CItemList GI 多 2 对象，
// 两侧同时 DIFF）。
std::vector<std::string> CFieldLengthChecker::fieldNames_;
std::vector<unsigned int> CFieldLengthChecker::fieldLength_;

#include "CFieldLengthChecker/CFieldLengthChecker.inc"
}

#include "CFieldLengthChecker/dtor_CFieldLengthChecker.inc"
}

#include "CFieldLengthChecker/AddField.inc"
}
