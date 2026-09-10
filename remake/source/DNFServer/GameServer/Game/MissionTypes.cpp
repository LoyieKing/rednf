// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MissionTypes/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <bitset>
#include "MissionTypes.h"


// [b28c-lane17] ORIG ctor 0x08a5ba8c：两枚 pair 成员默认构造（+0x10/+0x18）
// 由编译器在初始化列表自动发射，ctor 体仅调 clear()。
#include "MissionTypes/MissionScript.inc"
}


// ORIG 0x8a5aa4e（is_Victory_in_a_row.md）：连胜判定——mask=(1<<victory)-1，
// (bits & mask) == mask（低 victory 位全置位）。返回 bool：末尾直透
// operator== 的 al 结果，无 setne/movzbl 规范化（P9）。
// P112 封顶（b26 Lane27）：44 条指令/助记符全等；仅 3 个 bitset 局部槽位
// 非单调分配差（4.1 maskA@-0x14/maskB@-0xc/res@-0x10 vs 4.4 声明序
// -0xc/-0x10/-0x14），声明序换位会使 ctor 调用序反向，无合法源形，封顶。
// [b28c-L23 N93附 复测维持] 三枚 bitset 局部分配方向跨版本相反：4.1 先声明占**更深**
// 槽（maskA -0x14 / maskB -0xc / 结果 -0x10），4.4 先声明占**更浅**
// （-0xc/-0x10/-0x14）。本件已是「全头部声明」形（P212 前置条件满足仍不翻正），且
// 声明序绑死构造序（ORIG ctor 序 = maskA,maskB,operator&），翻转声明即翻转指令序
// ⇒ 无合法源形。44=44、助记符序列全等，仅 3 处 lea 槽号差。
// [b28d-23 N·N93附] 三枚 bitset 局部分配方向跨版本相反（4.1 先声明占深槽 -0x14/-0xc/-0x10 vs 4.4 -0xc/-0x10/-0x14），声明序绑死构造序 ⇒ N。ORIG 0x08a5aa4e
#include "MissionTypes/is_Victory_in_a_row.inc"
}


// ORIG 0x85e68b6 W：4×movw 清零（kind/index/count/streak）
#include "MissionTypes/clear.inc"
}

