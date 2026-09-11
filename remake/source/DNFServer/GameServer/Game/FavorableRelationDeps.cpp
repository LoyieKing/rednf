// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 FavorableRelationDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ stFavorableRelationShip_t 嵌套类型生命周期 producer。
//
// worklist（/tmp/only_orig/stFavorableRelationShip_t.txt，33 符号）依据：
//   docs/class_func_reports/stFavorableRelationShip_t*/ 各报告 + ORIG nm -S/objdump。
// 关键 ORIG 地址：
//   stRewardItemElement_t D1/D2      W 0x08a06f28 sz5   （纯 ret，元素平凡）
//   stBuffRewardInfo      C1/C2      W 0x085818f0 sz0x20（三标量清零）
//                         D1/D2      W 0x08a073d4 sz5   （平凡）
//   stLevelRewardInfo     C1/C2      W 0x0858188a sz0x65（两 string ctor + 三标量清零）
//                C1/C2ERKS          W 0x08a04160 sz0x83（标量拷贝 + 两 string 拷贝构造）
//                D1/D2              W 0x085823c6 sz0x4b（逆序两 string dtor）
//                operator=          W 0x08a04268 sz0x5f
//   stPeriodRewardInfo    C1/C2      W 0x08a00c70 sz0xe7（string@+4/+0x10/+0x14 ctor、
//                                                  标量清零 word@0/int@8/int@c、三 string 赋 ""）
//                         D1/D2      W 0x085819e4 sz0x73（0x14→0x10→0x4 逆序 string dtor）
//   stRewardItemInfo_t    C1/C2      W 0x08a00d58 sz0x73（3×vector<element> 默认构造）
//                         D1/D2      W 0x08581a80 sz0x2e（3 vector 逆序析构）
//                         clear()    T 0x089fb4c0 sz0x48（3×(vector clear + weight=0)）
//   stRewardItem_t        C1/C2      W 0x08a00dcc sz0xe4（15 个 Info 行主序构造）
//                         D1/D2      W 0x08581aae sz0x31（内联逐 Info 析构）
//                         clear()    T 0x089fb508 sz0x63（[5][3] 双循环 Info::clear）。
//
// 另含 Ghidra 伪命名伪类 stFavorableRelationShip_t::_140 / ._199 的六个符号
// （无法由合法 C++ 标识符产生）：
//   ._140 C1 @0x089fb8b6 / C2 别名：构造 (this)=MOOD-对 vector、(this+0xc)=ulong-对 vector；
//         D1 @0x089fb8fc：先 this+0xc 后 this 逆序析构。
//   ._199 D1 @0x0858046c：同布局双 vector 逆序析构（另一模板实例快照）。
// 以 extern "C" 函数 + GCC asm 标签提供与 ORIG 汇编一致的组合对象生命周期体；
// 组合块布局 {vector<pair<ENUM_NPC_MOOD,uint>>; vector<pair<ulong,uint>>} 取证自
// ._199 报告反编译（this+0xc 处为 vector<pair<unsigned_long,unsigned_int>>，
// this 处为 vector<pair<ENUM_NPC_MOOD,unsigned_int>>），与主结构
// m_moodChanges(+0x10)/m_keyItems(+0x1c) 的元素类型一致。

#include "NpcScript.h"

#include <new>

namespace
{

// [OurOnly33 全局符号轴 2026-09-11] ORIG 全 ELF 无 NPC_FAVOR_REWARD_SLOTS：我方曾发射
// `_ZN12_GLOBAL__N_1L22NPC_FAVOR_REWARD_SLOTSE`（r 段 4B）；本 TU 内**无任何使用点**
//（grep 仅命中本定义）⇒ 直接删除（-fkeep-static-consts 默认因此前仍发射符号）。

}  // namespace

// ---- stRewardItemInfo_t ---------------------------------------------------

// ORIG 0x08a00d58：循环默认构造 3 个 vector<stRewardItemElement_t>（stride 0xc）；
// 权重计数区（unsigned int[3]）无初始化动作。
#include "FavorableRelationDeps/stRewardItemInfo_t.inc"
{
}

// ORIG 0x08581a80：自尾向首逐个析构 3 个 vector（步长 -0xc）。由隐式数组成员
// 析构达成同一访问序列。
#include "FavorableRelationDeps/dtor_stRewardItemInfo_t.inc"
{
}

// ORIG 0x089fb4c0 T：j 0..≤2 —— vector.clear(j*0xc) 后 weight[j]=0（写位于槽尾
// 0x24+4j，即本结构的权重计数字段）。
#include "FavorableRelationDeps/clear.inc"
{
    for (int j = 0; j <= 2; ++j)
    {
        m_byDir[j].clear();
        m_weight[j] = 0;
    }
}

// ---- stRewardItem_t -------------------------------------------------------

// ORIG 0x08a00dcc：行主序构造 15 个 stRewardItemInfo_t（外层 [5] stride 0x90）。
#include "FavorableRelationDeps/stRewardItem_t.inc"
{
}

// ORIG 0x08581aae：编译器生成的逐 Info 析构展开；隐式数组成员析构等价。
#include "FavorableRelationDeps/dtor_stRewardItem_t.inc"
{
}

// ORIG 0x089fb508 T：外层 i 0..≤4（stride 0x90）、内层 j 0..≤2（stride 0x30），
// 逐槽 m_slots[i][j].clear()。
#include "FavorableRelationDeps/clear_1.inc"
{
    for (int i = 0; i <= 4; ++i)
    {
        for (int j = 0; j <= 2; ++j)
        {
            m_slots[i][j].clear();
        }
    }
}

// ---- stLevelRewardInfo ----------------------------------------------------

// ORIG 0x0858188a：string@+0xc、string@+0x10 默认构造后 level/flag0/flag1 清零。
#include "FavorableRelationDeps/stLevelRewardInfo.inc"
{
    m_level = 0;
    m_flag0 = 0;
    m_flag1 = 0;
}

// ORIG 0x08a04160：word/dword/dword 标量拷贝 + 两 string 拷贝构造（+0xc/+0x10）。
// [b28d-41] P239 复现：标量三段须与两 string 拷贝同列初始化列表且按声明序发射；
// 原「列表只 string、标量写 ctor 体」形使 4.4 先构造 string 后拷贝标量（块序反）。
stFavorableRelationShip_t::stLevelRewardInfo::
    stLevelRewardInfo(const stLevelRewardInfo& other)
    : m_level(other.m_level), m_flag0(other.m_flag0), m_flag1(other.m_flag1),
      m_name(other.m_name), m_desc(other.m_desc)
{
}

// ORIG 0x085823c6：m_desc(+0x10)、m_name(+0xc) 逆序析构（与隐式析构一致）。
#include "FavorableRelationDeps/dtor_stLevelRewardInfo.inc"
{
}

// ORIG 0x08a04268：标量逐位赋值 + 两 string 基本赋值（无自赋值检查）。
stFavorableRelationShip_t::stLevelRewardInfo&
stFavorableRelationShip_t::stLevelRewardInfo::
    operator=(const stLevelRewardInfo& other)
{
    m_level = other.m_level;
    m_flag0 = other.m_flag0;
    m_flag1 = other.m_flag1;
    m_name = other.m_name;
    m_desc = other.m_desc;
    return *this;
}

// ---- stBuffRewardInfo -----------------------------------------------------

// ORIG 0x085818f0：word@0、word@2、dword@4 全部清零。
#include "FavorableRelationDeps/stBuffRewardInfo.inc"
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
}

// ---- stPeriodRewardInfo ---------------------------------------------------

// ORIG 0x08a00c70：三个 string（+0x4/+0x10/+0x14）默认构造 → 标量清零
// （word@0、int@8、int@c）→ 三个 string 依次赋 ""（ORIG 空串常量 @0x08e1ce20）。
#include "FavorableRelationDeps/stPeriodRewardInfo.inc"
{
    m_type = 0;
    // [OurOnly33 符号名轴 2026-09-11] ORIG 全 ELF 无 `kEmptyString`（我方发射
    // `_ZZN25stFavorableRelationShip_t18stPeriodRewardInfoC1EvE12kEmptyString`，ORIG None）——
    // ORIG 三处直接以匿名空串常量 @0x08e1ce20 赋 string ⇒ 删 local static、使用点内联字面量
    // （空串仍落匿名 .rodata，装载形与 ORIG 一致）。
    m_name = "";
    m_a = 0;
    m_b = 0;
    m_desc = "";
    m_param = "";
}

// ORIG 0x085819e4（W 0x73）：空体（三 string 成员按逆序 0x14→0x10→0x4 自动析构）。
#include "FavorableRelationDeps/dtor_stPeriodRewardInfo.inc"
{
}

// ---- stFavorableRelationShip_t::._140 → 真匿名 struct（NpcScript.h 内 m_fav）----
// ORIG _ZN25stFavorableRelationShip_t5._140{C1,C2,D1,D2}Ev t 0x089fb8b6 / 0x089fb8fc（各 sz 0x45）。
// [AnonLane round8 2026-09-11] 原命名类 `t_140` + 空体 ctor/dtor 定义 + `.set` 别名已全部退役：
// 匿名 struct 的**隐式** ctor/dtor 与 ORIG 同形（默认构造/逆序析构两个 vector<pair>），
// 名字由编译器**直发** `._N`（N 由本 TU 的 FAV_ANON_140_PAD padding 垫到 140）⇒
// nm 轴与 p167 重定位轴同时对齐（GiantB `DisjointScript::._381` 先例）。

// [AnonLane round8 2026-09-11] 原 `dtor_t_199.inc`（P203 命名类 t_199 的析构体定义）已退役：
// ORIG 的 `._199D1/D2Ev`（t 0x0858046c）归 **NPCDynamicInfo 系 TU**，改由 NPCDynamicInfo.cpp 的
// 析构强制点 + `FAV_ANON_199_EXTRA` 编号垫自然发射（见该文件与 NpcScript.h 两处判词）。

// [AnonLane round8 2026-09-11] 原 `._140`/`._199` 的 `.set` 别名块（`L8_ALIAS` 宏 + 六条 + `#undef`）
// **整块退役**：两处真名现均由编译器自然发射 —— `._140` ← NPCScript TU（NpcScript.h 通用编号垫 53 个
// 哑匿名类型）、`._199` ← NPCDynamicInfo TU（`FAV_ANON_199_EXTRA` 垫 54 个）。
// 退役理由（本轮实测）：`.set` 只改 nm 轴与反汇编**显示名**，**改不了重定位名**（p167 恒
// MISSING/EXTRA）；且 ORIG 的 `._NNN` 是 t（local）符号、归属唯一 TU，别名法会把符号伪造到
// 错误 TU（`._140` 实属 NPCScript、`._199` 实属 NPCDynamicInfo）。真匿名 struct + 计数垫
// 同时满足 nm 轴、重定位轴与归属（GiantB `DisjointScript::._381` 先例）。


// [AnonLane round8 2026-09-11] `._140` 的**本 TU 侧副本**发射点：本 TU 在 ORIG 中同样有
// m_fav 匿名副本的贡献（P203 别名时代它以 `._140` 别名声发射，退役后需由真匿名类型补回），
// 用 `delete p` 只发析构（D1/D2），编号由 NpcScript.h 的通用垫（53 个）使本 TU 亦为 140。
void __favAnon140ForceClear(stFavorableRelationShip_t* p)
{
    delete p;
}
