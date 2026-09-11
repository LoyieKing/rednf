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

#include "CItemMakingSkill/GetPrevExpGap.inc"

