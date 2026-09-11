// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFLexWrapper/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "DNFLexWrapper.h"
#include "ScriptStringManager.h"
#include "SyncScriptDeps.h"

extern "C" double __strtod_internal(const char*, char**, int);
extern "C" long __strtol_internal(const char*, char**, int, int);

#include "DNFLexWrapper/DNFLexWrapper.inc"


#include "DNFLexWrapper/setLexType.inc"


#include "DNFLexWrapper/getLexType.inc"


#include "DNFLexWrapper/getLastToken.inc"


#include "DNFLexWrapper/getLastTokenData.inc"


#include "DNFLexWrapper/getLastLineNo.inc"


#include "DNFLexWrapper/getType.inc"


#include "DNFLexWrapper/getConnector.inc"


#include "DNFLexWrapper/getCustomData.inc"


#include "DNFLexWrapper/getString.inc"


#include "DNFLexWrapper/getToken.inc"


// [b28d-solo-DNFLexWrapper] N 封顶（跨版本寄存器分配 + 块布局，N7/N18a/N82 族）：ORIG 0x08ace2f0
// （-O2，68 insn）把 c 分配 %ecx 并复用为 d（`movsbl %cl,%ecx; sub $0x30,%ecx`），4.4.6 恒分配
// %eax（`movsbl %al,%eax; lea -0x30(%eax),%ecx`，mnem-ndiff 26/64）；v=0 被 4.4 下沉到 `ja` 之后
// （ORIG 在 parse 入口）、minus 块排在 skip_space 之前、maybe_neg 就地复制 epilogue
// （ORIG `pop/pop/ret` vs 我们 `jmp` 复用）。
// 已试（均 ≥26，最佳即当前 while 形）：optimize 属性矩阵 O0/O1/O2/O3 + no-if-conversion/
// no-omit-frame-pointer/no-crossjumping/no-thread-jumps/no-tree-sink/no-reorder-blocks/
// no-schedule-insns2/ira-algorithm=cb/ira-region=one|all/no-tree-pre/no-tree-ccp、变量声明序/
// register/signed char、v 位置三形（+v/dthenv/for）、Ghidra do-while 结构重写、三目/negassign。
// P167 CLEAR(0/0) 语义等价；§6.1 对齐 nop 已剥。
__attribute__((optimize("O2,no-if-conversion,no-omit-frame-pointer")))
#include "DNFLexWrapper/getDecimal.inc"


#include "DNFLexWrapper/getHexDecimal.inc"


#include "DNFLexWrapper/getFloat.inc"


// getPairInt（ORIG 0x8ace630）：NORMAL 对 yytext+1 拷贝 0x100 字节并保证终止后
// strtol base 10（跳过成对 token 前导字符，如 "=123"）；COMPILED 返回
// lastIntValue_；其它 lexType 返回 0。
// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace630；-O2 路由下 4.4 内联 strlen 为 word-at-a-time
// （与 ORIG 同形），残余 cmov 段由 target("arch=i486") 消除（P230①）⇒ 54=54 逐条同形（P167 CLEAR 2/2）。
__attribute__((target("arch=i486")))
#include "DNFLexWrapper/getPairInt.inc"


// [b28d-solo-DNFLexWrapper] AE：ORIG 0x08ace5a0；target("arch=i486") 消 cmov ⇒ 41=41 逐条同形
// （.bss 静态 buffer 的 R_386_32 绝对地址按 AE 口径归一 <A>；P167 CLEAR 1/1）。
__attribute__((target("arch=i486")))
#include "DNFLexWrapper/getPairStr.inc"


#include "DNFLexWrapper/getLastTokenDataAsString.inc"


#include "DNFLexWrapper/setStringTableFromBuffer.inc"


#include "DNFLexWrapper/setStringTableFromFile.inc"


#include "DNFLexWrapper/SetHandler.inc"


#include "DNFLexWrapper/dtor_DNFLexWrapper.inc"

