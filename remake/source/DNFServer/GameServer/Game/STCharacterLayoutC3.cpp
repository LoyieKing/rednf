// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STCharacterLayoutC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "STCharacterScript.h"

// [b28d-51] 判词 N（ORIG 0x083678a0，N82/N91 族）：STSecondGrowTypeData::C1 为
// STCharacterScript.h L127 头内 inline（clear() 体），7× 数组构造链槽/帧差；
// 语义/构造集齐，byte 层封顶。
// [b28d-51] 判词 N（ORIG 0x08368348，N82/N91 族）：vector/string 成员构造 +
// EH cleanup 残段跨版本差；语义/构造集齐，byte 层封顶。
// [P2S5] 负结论（ORIG 0x08368348，848 条）：与本 TU diag 仅差 1 条 —— EH landing
// pad 内 ORIG 为 `mov %edi,%eax; mov %esi,%edx;` ▸ `mov %edx,%esi; mov %eax,%edi;`
// 连续 fall-through（0x08368bxx 段），我方多一条 `jmp <下一 LP>`（n1_v 残差
// insert orig[821]，our 849）。试旋钮「内层作用域包裹 Clear()」无效（849→849，
// 同一位置同一 jmp）⇒ 该 jmp 属 GCC 4.4 对多个 landing pad 的分块/前驱判定，
// 源码层不可控（函数体语义/构造集齐，仅差此 1 条 EH 序）⇒ 封 N。
#include "STCharacterLayoutC3/STCharacterScript.inc"
{
    Clear();
}

// [b28d-51] 判词 N（ORIG 0x08376362，N82/N91 族）：成员析构链 + EH 尾块序
// （N104）跨版本差；语义/析构集齐，byte 层封顶。
#include "STCharacterLayoutC3/dtor_STCharacterScript.inc"
{
}
