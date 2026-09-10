// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCompoundAvatar/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射。
// [T1-GI · P327] 键名 `_GLOBAL__I__ZN8WongWork15CCompoundAvatarC2Ev`（ORIG 0x8334202）：
// 4.4 实测首强符号 = STAncientRareAvatarCube C1（**类外定义 ⇒ 强符号 T**，非弱；
// 其 W 仅出现于编译器的 copy/D1 生成件）⇒ WongWork::CCompoundAvatar C1 定义必须
// 前移到它之前，见下方「键名位」块。
#include <iostream>

#include "CCompoundAvatar.h"
#include "PvPTypes.h"      // CMTRand 完整类型（0x9c8）

#include <ctime>
#include <cstring>

// ==================== WongWork::CCompoundAvatar（键名位 · P327）====================
// ORIG 0x08331d74：11×tagCompoundAvatarTable 构造（成员数组自动）→
// CMTRand(new 0x9c8)(time(0)) 存 +0x84 → 100 次 randInt(10000) 热身。
// [N5Free211to230.N5D 2026-09-11] 槽序已解（DIFF→AE）：ORIG 0x08331deb seed=-0x24 /
// 0x08331e35 range=-0x20。range 原为循环内具名局部（4.4 落深槽 -0x24，与 seed 临时
// 对调）⇒ 改为内联右值临时 `randInt(static_cast<unsigned long>(10000))` 后，4.4 按
// 「匿名临时先创建者占深槽」使 seed 临时 -0x24 / range 临时 -0x20，与 ORIG 逐槽一致
// （探针 /tmp/r5/n5/p_avatar5.cpp fs：4.4 输出与 ORIG 0x08331deb..0x08331e35 同形）。
// 旧 P212/N82「槽序封顶」判词作废。
namespace WongWork
{
#include "CCompoundAvatar/CCompoundAvatar.inc"
{
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        // ORIG 0x08331e35：range 为绑定到 `const unsigned long&` 的匿名临时
        // （movl $0x2710,-0x20(%ebp) + lea 取址），非具名局部。
        m_rand->randInt(static_cast<unsigned long>(10000));
    }
}
}  // namespace WongWork

// ============================ STAncientRareAvatarCube ============================

// ORIG 0x0891aa3a：单循环内逐组「置零 int + clear 向量」，末尾 clear 首向量。
#include "CCompoundAvatar/STAncientRareAvatarCube.inc"
{
    for (int i = 0; i < 10; ++i)
    {
        m_field0c[i] = 0;
        m_vecs[i].clear();
    }
    m_vec.clear();
}

// ============================ tagCompoundAvatar ============================

// ORIG 0x0891ab56：标量清零（含 memset 0x28 于 +0x04）+ 三组/尾向量逐个 clear，
// 循环体内顺序为 A.clear/B.clear/m_b/m_c/m_d/C.clear，尾部顺序 2b8/2dc/2e0[]/2c4/2d0。
#include "CCompoundAvatar/Clean.inc"
{
    m_idx = 0;
    std::memset(&m_a[0], 0, sizeof(m_a));
    for (int i = 0; i < 10; ++i)
    {
        m_optsA[i].clear();
        m_optsB[i].clear();
        m_b[i] = 0;
        m_c[i] = 0;
        m_d[i] = 0;
        m_optsC[i].clear();
    }
    m_tailA.clear();
    m_flag2dc = 0;
    for (int j = 0; j < 10; ++j)
    {
        m_e[j] = 0;
    }
    m_tailB.clear();
    m_tailC.clear();
}

// ============================ tagCompoundAvatarTable ============================

#include "CCompoundAvatar/tagCompoundAvatarTable.inc"

#include "CCompoundAvatar/dtor_tagCompoundAvatarTable.inc"

// ORIG 0x08903fcc：公共路径内联、非法路径前向跳转合并 ret0；
// at(idx).m_idx != idx → 0；否则元素指针。
tagCompoundAvatar*
#include "CCompoundAvatar/getCompoundAvatarInfo.inc"
{
    // b28d-45 P0/N96 清绑：`register bool invalid asm("al")` 删除，裸短路形
    // （ORIG (idx<1)||(size()<=idx) bool 物化岛 → test/je；body 内 at() 落槽
    // -0xc，*entry==idx je 正跳 return-块）。
    if ((idx < 1) || (static_cast<int>(m_avatars.size()) <= idx))
        return 0;
    const tagCompoundAvatar* entry =
        &m_avatars.at(static_cast<unsigned int>(idx));
    if (entry->m_idx != idx)
        return 0;
    return const_cast<tagCompoundAvatar*>(entry);
}

// ============================ WongWork::CCompoundAvatar ============================

namespace WongWork
{

// ORIG 0x08331ea2：delete m_rand 并置空；11 张表逆序析构（编译器生成）。
#include "CCompoundAvatar/dtor_CCompoundAvatar.inc"
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}

// ORIG 0x08333ea6：importCompoundAvatarTable(dir,path,this) == 0。
#include "CCompoundAvatar/LoadScript.inc"
{
    return importCompoundAvatarTable(dir, path,
                                     reinterpret_cast<tagCompoundAvatarTable*>(this)) == 0;
}

}  // namespace WongWork
