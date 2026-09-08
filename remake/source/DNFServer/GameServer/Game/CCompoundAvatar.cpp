// ============================================================================
// CCompoundAvatar.cpp — 头像合成/分离脚本域生命周期与查询（Ctor2_ContB）
//
// 全部实现按 docs/class_func_reports/{WongWork__CCompoundAvatar,
// tagCompoundAvatarTable}/ 报告 + ORIG objdump 逐段转写：
//   STAncientRareAvatarCube C1 W 0x0891aa3a（copy/D1 由编译器生成，与 ORIG W 等价）
//   tagCompoundAvatar       Clean W 0x0891ab56（特member 函数由编译器生成，
//                           逐成员顺序与 ORIG W 0x0891be88/0x0891c1f8/
//                           0x0833463c/0x0894ec3a 一致）
//   tagCompoundAvatarTable C1 W 0x0833431c / D1 W 0x08334330 /
//                          getCompoundAvatarInfo W 0x08903fcc
//   WongWork::CCompoundAvatar C1 T 0x08331d74 / D1 T 0x08331ea2 /
//                          LoadScript T 0x08333ea6
// importCompoundAvatarTable(T 0x08903ea7) 仅声明——定义归脚本导入域批次。
// ============================================================================
#include "CCompoundAvatar.h"
#include "PvPTypes.h"      // CMTRand 完整类型（0x9c8）

#include <ctime>
#include <cstring>

// ============================ STAncientRareAvatarCube ============================

// ORIG 0x0891aa3a：单循环内逐组「置零 int + clear 向量」，末尾 clear 首向量。
STAncientRareAvatarCube::STAncientRareAvatarCube()
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
void tagCompoundAvatar::Clean()
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

tagCompoundAvatarTable::tagCompoundAvatarTable() {}   // ORIG 0x0833431c：vector 构造

tagCompoundAvatarTable::~tagCompoundAvatarTable() {}  // ORIG 0x08334330：vector 析构

// ORIG 0x08903fcc：公共路径内联、非法路径前向跳转合并 ret0；
// at(idx).m_idx != idx → 0；否则元素指针。
tagCompoundAvatar*
tagCompoundAvatarTable::getCompoundAvatarInfo(int idx) const
{
    // ORIG: (idx<1)||(size()<=idx) bool 物化岛（mov$1/jmp/mov$0 落 al，P194 register 形）
    // → test/je；body 内 at() 落槽 -0xc，*entry==idx je 正跳 return-块。
    register bool invalid asm("al") =
        (idx < 1) || (static_cast<int>(m_avatars.size()) <= idx);
    if (invalid)
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

// ORIG 0x08331d74：11×tagCompoundAvatarTable 构造（成员数组自动）→
// CMTRand(new 0x9c8)(time(0)) 存 +0x84 → 100 次 randInt(10000) 热身。
// 槽残差：ORIG seed=-0x24/range=-0x20 vs 4.4 call 临时恒浅槽对调（P212 全头部
// 声明试形反劣 8→10，具名 seed 亦不落深槽）——与 CItemUpgrade_Separate::C1
// b26 判词同型，N82 族封顶。
CCompoundAvatar::CCompoundAvatar()
{
    // b28d-45 复测：P212 全头部声明（seed/range/i 按 ORIG 槽深序）槽序不变且
    // 判级 NEAR→DIFF，确认 N82/N98 槽序封顶（ORIG 0x08331deb seed=-0x24 /
    // 0x08331e35 range=-0x20 不可翻）。
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        unsigned long range = 10000;
        m_rand->randInt(range);
    }
}

// ORIG 0x08331ea2：delete m_rand 并置空；11 张表逆序析构（编译器生成）。
CCompoundAvatar::~CCompoundAvatar()
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}

// ORIG 0x08333ea6：importCompoundAvatarTable(dir,path,this) == 0。
bool CCompoundAvatar::LoadScript(const char* dir, const char* path)
{
    return importCompoundAvatarTable(dir, path,
                                     reinterpret_cast<tagCompoundAvatarTable*>(this)) == 0;
}

}  // namespace WongWork
