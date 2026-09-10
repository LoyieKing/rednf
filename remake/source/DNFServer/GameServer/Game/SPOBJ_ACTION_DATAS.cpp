// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SPOBJ_ACTION_DATAS/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r SPOBJ_ACTION_DATAS 成员定义（ORIG 符号均为 W，头内联；本 TU 外联）。
// ORIG 证据（init/df_game_r）：
//   C1 0x089e86e4: memset(this, 0xff, 0x18) —— movl $0x18,0x8(%esp);
//                  movl $0xffffffff,0x4(%esp); 全字段初始化为 -1。
//   D1 0x089f0b92: push ebp/mov esp,ebp/pop ebp/ret（空体）。
//   getActionType 0x089e8708: mov 0x8(%ebp),%eax; mov (%eax),%eax; ret ——
//                  非 const 成员、返回 int，直返 m_type（+0x00）。
#include <cstring>
#include "SPOBJ_ACTION_DATAS.h"

#include "SPOBJ_ACTION_DATAS/SPOBJ_ACTION_DATAS.inc"
{
    memset((char*)this, -1, 0x18);
}


#include "SPOBJ_ACTION_DATAS/getActionType.inc"
{
    return m_type;
}
