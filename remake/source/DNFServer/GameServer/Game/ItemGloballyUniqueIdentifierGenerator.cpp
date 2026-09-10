// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ItemGloballyUniqueIdentifierGenerator/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CItemGloballyUniqueIdentifierGenerator / itemGloballyUniqueIdentifier_t
// ORIG 证据（df_game_r nm -S / objdump -d，报告缺失按 §3.2 objdump 直转）：
//   reset()  W 0x085139e8：char 循环 i=0..9 把 this 前 10 字节清零（setle 判界）。
//   generate T 0x0889246c：SetServerId(field)；SetDatetime(time(0))；
//            SetSequenceNo((ushort)getSeq())。
//   getSeq   W 0x088947e2：++m_seq；若 (unsigned)m_seq > 0xea5f 则归零；返回。
#include "ItemGloballyUniqueIdentifierGenerator.h"
#include <time.h>   // time（generate SetDatetime）

// [b28d-D43] N4 scaled 写 vs lea 折叠：ORIG `movb $0,(%edx,%eax,1)` vs 4.4 `lea (%edx,%eax,1); movb $0,(%eax)`（ORIG 0x08899dfa）。
#include "ItemGloballyUniqueIdentifierGenerator/reset.inc"
{
    // ORIG 0x85139e8：循环变量为 char（movsbl 读/addb 自增/cmpb $9 判界）。
    // [b28c-33 判词 NEAR-N4] ORIG 写侧为 `movsbl i,%eax; mov this,%edx; movb $0,
    // (%edx,%eax,1)`（索引先+scaled 直存）；4.4 对 `(char*)this[i]` 恒折
    // `mov this,%edx; movsbl i,%eax; lea (%edx,%eax,1); movb $0,(%eax)`（+1 条）。
    // 探针 l33d f1..f5：仅「成员数组」形出 scaled，本类无 +0 起 char[10] 成员
    // （需 union 视图改头，收益 1 条，未做）。15/16。
    for (char i = 0; i <= 9; ++i)
        reinterpret_cast<char*>(this)[i] = 0;
}

#include "ItemGloballyUniqueIdentifierGenerator/getSeq.inc"
{
    ++m_seq;
    if (m_seq > 0xea5f)
        m_seq = 0;
    return m_seq;
}

#include "ItemGloballyUniqueIdentifierGenerator/generate.inc"
    itemGloballyUniqueIdentifier_t* id, int field)
{
    id->SetServerId(field);
    id->SetDatetime(static_cast<int>(time(0)));
    id->SetSequenceNo(getSeq());
    return id;   // ORIG 0x88947e2 附近末尾 mov 0xc(%ebp),%eax
}

CItemGloballyUniqueIdentifierGenerator g_ItemGloballyUniqueIdentifierGenerator;
