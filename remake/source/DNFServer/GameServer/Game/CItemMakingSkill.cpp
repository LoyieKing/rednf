// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemMakingSkill/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [b28d-solo-CItemMakingSkill] verdict: getData=IDENTICAL_AE、GetPrevExpGap=IDENTICAL_AE
// （check_tu_routed：identical+ae=4 / diff=0 / near=0；两件残余仅 .o 未解析重定位目标，
//   clean 归一后与 ORIG 逐条相等）。
// CItemMakingSkill.cpp —— CItemMakingSkill::getData(char*) const
// （OrigItemMaking15 批；本 TU 独占 CItemMakingSkill.cpp/.h）。
//
// ORIG T 0x08545b42（df_game_r objdump 逐指令转写，范围 0x08545b42..0x85b7d）：
//   sub $0x10；out = pData（-0x8）；i = 0（-0x4）；
//   do 循环 i<=0xa（cmpl $0xa; setle）：
//     movzwl 0x6(%this,%i,2),%cx（m_skillLevels[i] 零扩展）
//     mov %cx,0x4(%out,%i,2)（目标 pData+4+i*2，ushort 16 位写）
//
// SIG_ITEM_MAKING_SKILL 布局（_saveData 0x08545276 佐证）：characNo@+0（4 字节，
// 由 _saveData 填 get_charac_no）＋ m_skillLevels[11]@+4（ushort ×11），总 0x1c。
// getData 仅序列化技能表段，characNo 不由本函数写。
// 循环写法照 P2（cmpl $imm; setle ⇔ for(int i=0; i<=N; ++i)）。
//
// [b28d-solo-CItemMakingSkill] 修复记录（旧 [b28d-D43]/[b28-L14] 的 N81/N4
//   「scaled store 不可折」判词作废）：ORIG 0x08545b57 循环体
//   `mov -0x4,%edx; mov -0x4,%ecx; mov 0x8(%ebp),%eax; movzwl 0x6(%eax,%ecx,2),%ecx;
//   mov -0x8(%ebp),%eax; mov %cx,0x4(%eax,%edx,2)` 的复现钥匙是**成员数组视图**
//   （ItemMakingSkillSaveView::m_skillLevels 位于 +0x04），不是 `ptr[i+2]` 常数下标：
//   P30 判据——成员视图 offset>0 由 4.4 折成单条 disp+scale，而指针+常数下标
//   恒展开 add/shl 链。读侧 `m_skillLevels[i]`（+0x06）两侧同为成员视图。
#include "CItemMakingSkill.h"

#include "CDataManager.h"   // G_CDataManager() / GetItemMakingSkill() / stItemMakingSkill

// [b28d-solo-CItemMakingSkill] SIG_ITEM_MAKING_SKILL 序列化视图（ORIG getData
// 0x08545b42 / _saveData 0x08545276 双证）：+0x00 characNo（4 字节，本函数不写）
// ＋ +0x04 m_skillLevels[11]（ushort ×11），总 0x1c（与 SigTypes_B.h
// SIG_ITEM_MAKING_SKILL 布局一致）。成员数组视图让 4.4 -O0 复现 ORIG 单条
// `mov %cx,0x4(%eax,%edx,2)`——P30 判据：成员视图 offset>0 单 disp 折叠，
// 而 `ptr[i+K]` 常数下标恒展开 add/shl 链（旧判词 N81/N4 作废）。
namespace
{
struct ItemMakingSkillSaveView
{
    int m_characNo;                    // +0x00
    unsigned short m_skillLevels[11];  // +0x04..+0x19
};
}  // namespace

#include "CItemMakingSkill/getData.inc"
{
    ItemMakingSkillSaveView* out = reinterpret_cast<ItemMakingSkillSaveView*>(pData);
    for (int i = 0; i <= 0xa; ++i)
        out->m_skillLevels[i] = m_skillLevels[i];
}
// ============================================================================
// CItemMakingSkill::GetPrevExpGap —— ORIG T 0x08545b7e（OrigItemGen20 批）。
// 报告 docs/class_func_reports/CItemMakingSkill/GetPrevExpGap.md：
//   G_CDataManager()->GetItemMakingSkill() 得技能经验表宿主（stItemMakingSkill，
//   +0x18 为 vector<pair<int,int>> 档位表）；idx = level - 1；
//   守卫 (unsigned)idx >= size() || idx == 0 → 返回 0（ORIG cmp/jbe + cmpl/jne
//   双判，jbe 为无符号比较）；否则返回 档位[idx].first - 档位[idx-1].first
//   （两次 vector::operator[]，idx 与 idx-1；ORIG mov (%eax),%ebx/sub 形态）。
// ============================================================================
// [b28d-solo-CItemMakingSkill] 修复记录（旧 [b28d-D43] 的 N82/N95「槽平移/idx 寄存器
//   直比不可复现」判词作废）：
//   ① 守卫按 ORIG 方向写 `size() <= (unsigned)idx || idx == 0`——P209：unsigned 比较
//      在 4.4 恒先载寄存器；写成 `(unsigned)idx >= size()` 多 `mov idx,%ebx` 且比较方向反。
//   ② idx 局部声明为 unsigned int（P218 扩展）：同时翻正 4.4 的
//      `cmp -0xc(%ebp),%eax`（内存直比、无中转）与主流程 receiver 的 ORIG 形
//      `mov table,%eax; lea 0x18(%eax),%edx; mov idx,%eax`。
//   ③ table 声明为 `const stItemMakingSkill*`（P107/P270 符号级 cv 判据）：
//      p167 差集实证 ORIG 调 `_ZNKSt6vector…ixEj`（const operator[]），
//      非 const 指针会发 `_ZNSt6vector…ixEj`（2 处符号级错配）。
//   语义：level<=0 ⇒ idx=0xFFFFFFFF ⇒ 命中守卫返回 0，与 ORIG 无符号比较逐位一致；
//   主流程 idx/idx-1 作 vector 下标同值。
#include "CItemMakingSkill/GetPrevExpGap.inc"
{
    const stItemMakingSkill* table =
        reinterpret_cast<const stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    unsigned int idx = static_cast<unsigned int>(level - 1);
    if (table->m_vec18.size() <= idx || idx == 0)
        return 0;
    return table->m_vec18[idx].first - table->m_vec18[idx - 1].first;
}
