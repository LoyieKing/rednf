// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PvpUserTable/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r PvpUserTable（G2-5 PvP 队伍表，0x34c 字节）
// 逐函数对照 docs/class_func_reports/PvpUserTable.md（汇编 + Ghidra C）与
// ORIG 反汇编实现；GetWinTeam/GetUserVsTeamPoint 为 ORIG 弱符号，直接读
// 0x85df7a4 / 0x85dfddc 反汇编实现。
// 布局（自 ORIG 构造/析构 + 各函数寻址推导）：
//   +0x000 float m_teamVsTeam[8][8]  队伍间分差矩阵（0x100）
//   +0x100 float m_teamPoint[8]      队伍期望分累计（0x20）
//   +0x120 m_users[8][8]（CUser*+uchar，8 字节/项，0x200）
//   +0x320 int m_winTeam
//   +0x324 float m_minPoint[8]      各队伍最小分（0x20，GetLeavePenalty 用）
//   +0x344 bool m_dan[8]            丹位标记（b28d-solo：P248 档② bool 化）
// 大小合计 0x34c。
// ============================================================================
// [G5 2026-09-10 · L12 round3 复核] 2 件全核（p167 + 判据⑪）：
//   · GetTeamVsTeamPoint：判据⑪ = (a) 相对序差（我方 -0x10..-0x4c ↔ ORIG -0xc..-0x48，
//     浅端多 4B/深端多 4B）；P227 全表达式临时链试验（ORIG `fstpl -0x48` 在 ctor 前）
//     实测四数与槽表逐值不变 ⇒ 回滚；残余 N85（fucompp/fnstsw/sahf ↔ 4.4 fucomip/fstp）
//     + N82 槽组（帧 0x70↔0x60），p167 CLEAR 7/7 ⇒ 语义完成。
//   · GetResult：判据⑪ = (b) 整帧平移（槽集合全等）⇒ N126 封顶，p167 仅别名差。

#include <algorithm>
#include <cstring>

#include "PvPTypes.h"  // PvpUserTable 权威完整定义（本 TU 唯一实现归属）

#include "CUserCharacInfo.h"
#include "GameWorld.h"

class CUser;

// cMyTrace 权威声明见 GameTypes.h（经 GameWorld.h 链包含），本地类已删除（§7）。


// ORIG GetResult/GetLeavePenalty 以 fnstcw→mov $0xc,%ah（RC=11 截断）→fldcw→
// fistpl 转 int（0x85d51bd 等实证），即 C 的 float→int 截断转换，非 ±0.5 舍入。
#include "PvpUserTable/PvpUserTable.inc"
}

#include "PvpUserTable/dtor_PvpUserTable.inc"
}

#include "PvpUserTable/Clear.inc"
}

// [b28d-54 N] ORIG 0x085d4980：帧 0x24→0x28 + `push %ebx` 差（ORIG ebx 缓存
//   成员基址）——4.4 寄存器分配（N7/N82）。语义等价。
#include "PvpUserTable/Set.inc"
}

#include "PvpUserTable/IsValidTeam.inc"
}

// b28d-54 判词：ORIG 0x085d4a32（GetExpectVal 两处调用点 0x085d4b4a/0x085d4b5f 均
//   `call; xor $1,%eax; test %al,%al; je`）⇒ 返回 bool（P248 档①）。
#include "PvpUserTable/IsValidConvertedGrade.inc"
}

#include "PvpUserTable/GetTeamUserCount.inc"
}

// b28d-solo-PvpUserTable：ORIG 0x085d4ab8，AE。翻正：空 then 的
//   `if (user != 0 && grade <= 9) {} else {回退}` 使 4.4 物化成功值
//   （`mov $1/jmp/mov $0/test %al,%al/jne`，P272 德摩根块序）；此前
//   `!(A&&B)` 与 `A'||B'` 两形恒物化失败值（jle/je 反极性）。
#include "PvpUserTable/GetExpectVal.inc"
}

// b28d-solo-PvpUserTable：ORIG 0x085d4bd0。三处翻正——
//   ① 0x085d4bd7 `mov 0x8(%ebp),%eax` 先于两个常量压栈 ⇒ P68 `(char*)this`；
//   ② 0x085d4caa 段先算第二实参（&m_teamVsTeam[j][i]）再算第一实参
//      （&m_minPoint[i]，`lea 0x4(%edx)`）⇒ std::min 实参序为
//      (m_minPoint[i], m_teamVsTeam[j][i])；
//   ③ 0x085d4c64 空 then 占位岛（`je +0x94(nop)` + body 尾 `jmp +0x95`）⇒
//      P223 正条件 goto 尾空标签形。
#include "PvpUserTable/Calculate.inc"
}

// b28d-solo-PvpUserTable：ORIG 0x085d4d5e，AE（其余件）⇒ 本件 byte 层 N 封顶。
//   翻正——
//   ① 0x085d4db6 `movzbl flag; xor $1; test %al; je` ⇒ `!flag`（字段已 bool 化）；
//   ② 0x085d4e6b `mov -0x28,%eax; cmp -0x24,%eax; jle`（count1 vs count2）⇒
//      条件写 `count1 > count2`（写 count2<count1 时 4.4 载 count2 反序）；
//   ③ 0x085d4f38 ratio 段先 `fstps -0x1c` 存局部再 `flds -0x1c` 回读 ⇒ 两条语句；
//   ④ 零分支 0x085d4eee `fstpl -0x48` 在 ctor 前 ⇒ 具名 double 局部（N80 求值序）。
//   残余：①0x085d4ed2 fucompp/fnstsw/sahf vs 4.4 fucomip/fstp（N85 x87 比较
//   lowering，-march=i486 已生效仍不可翻）；②槽号整体深 8/帧 0x70 vs 0x60
//   （N82；4.4 把 double 临时放浅槽 -0x10 vs ORIG -0x48）。指令 191 vs 194
//   （差即 N85 段）、p167 CLEAR 7/7 ⇒ 语义完成。
#include "PvpUserTable/GetTeamVsTeamPoint.inc"
}

#include "PvpUserTable/GetGhostVsTeamPoint.inc"
}

// b28d-solo-PvpUserTable：ORIG 0x085d5064，byte 层 N85 封顶。翻正——
//   ① m_dan bool 化（P248 档②，0x085d511d xor 形）；
//   ② 四处 trace 改全表达式临时链（P227：double 实参 ctor 前 `flds;fstpl`、
//      int 结果 `mov -0x74,%ebx` 跨 ctor 保活）；
//   ③ revision 分支物理序改 A(mode<=14)→B(mode>14)→C(m_winTeam==team)
//      （P222/P47；4.4 对独立 if+return 恒 then 放前）；
//   ④ 0x1dd trace 的 %d 实参修正为 `(int)m_teamVsTeam[...]`（ORIG
//      0x085d569e 无 ×1000 乘法，真语义差异）。
//   残余：0x085d5148 fucompp/fnstsw/sahf vs 4.4 fucomip/fstp（N85）；
//   467=467 条、帧 0xb4+push %ebx 全等、p167 CLEAR 13/13 ⇒ 语义完成。
#include "PvpUserTable/GetResult.inc"
}

#include "PvpUserTable/GetLeavePenalty.inc"
}

#include "PvpUserTable/GradeFilter.inc"
}

#include "PvpUserTable/CheckDan.inc"
}

// b28d-solo-PvpUserTable：ORIG 0x085d585e 两分支各自 `movb $0x1/$0x0` 直写
//   （then 落 0x085d589c、else 落 0x085d58b5，无 al 物化、无 push %ebx、
//   循环槽 -0x8/-0x4）⇒ 三目写形在 4.4 恒物化 `mov $1/jmp/mov $0` 并缓存 i 到
//   %ebx（P171 反向：ORIG 不物化就必须写成两分支独立赋值）。
#include "PvpUserTable/SetWinTeam.inc"
}

#include "PvpUserTable/GetWinTeam.inc"
}

#include "PvpUserTable/GetUserVsTeamPoint.inc"
}
