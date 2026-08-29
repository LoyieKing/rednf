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

const unsigned int NPC_FAVOR_REWARD_SLOTS = 3;

}  // namespace

// ---- stRewardItemInfo_t ---------------------------------------------------

// ORIG 0x08a00d58：循环默认构造 3 个 vector<stRewardItemElement_t>（stride 0xc）；
// 权重计数区（unsigned int[3]）无初始化动作。
stFavorableRelationShip_t::stRewardItemInfo_t::stRewardItemInfo_t()
{
}

// ORIG 0x08581a80：自尾向首逐个析构 3 个 vector（步长 -0xc）。由隐式数组成员
// 析构达成同一访问序列。
stFavorableRelationShip_t::stRewardItemInfo_t::~stRewardItemInfo_t()
{
}

// ORIG 0x089fb4c0 T：j 0..≤2 —— vector.clear(j*0xc) 后 weight[j]=0（写位于槽尾
// 0x24+4j，即本结构的权重计数字段）。
void stFavorableRelationShip_t::stRewardItemInfo_t::clear()
{
    for (int j = 0; j <= 2; ++j)
    {
        m_byDir[j].clear();
        m_weight[j] = 0;
    }
}

// ---- stRewardItem_t -------------------------------------------------------

// ORIG 0x08a00dcc：行主序构造 15 个 stRewardItemInfo_t（外层 [5] stride 0x90）。
stFavorableRelationShip_t::stRewardItem_t::stRewardItem_t()
{
}

// ORIG 0x08581aae：编译器生成的逐 Info 析构展开；隐式数组成员析构等价。
stFavorableRelationShip_t::stRewardItem_t::~stRewardItem_t()
{
}

// ORIG 0x089fb508 T：外层 i 0..≤4（stride 0x90）、内层 j 0..≤2（stride 0x30），
// 逐槽 m_slots[i][j].clear()。
void stFavorableRelationShip_t::stRewardItem_t::clear()
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
stFavorableRelationShip_t::stLevelRewardInfo::stLevelRewardInfo()
{
    m_level = 0;
    m_flag0 = 0;
    m_flag1 = 0;
}

// ORIG 0x08a04160：word/dword/dword 标量拷贝 + 两 string 拷贝构造（+0xc/+0x10）。
stFavorableRelationShip_t::stLevelRewardInfo::
    stLevelRewardInfo(const stLevelRewardInfo& other)
    : m_name(other.m_name), m_desc(other.m_desc)
{
    m_level = other.m_level;
    m_flag0 = other.m_flag0;
    m_flag1 = other.m_flag1;
}

// ORIG 0x085823c6：m_desc(+0x10)、m_name(+0xc) 逆序析构（与隐式析构一致）。
stFavorableRelationShip_t::stLevelRewardInfo::~stLevelRewardInfo()
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
stFavorableRelationShip_t::stBuffRewardInfo::stBuffRewardInfo()
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
}

// ORIG 0x08a073d4：平凡空体析构。
stFavorableRelationShip_t::stBuffRewardInfo::~stBuffRewardInfo()
{
}

// ---- stPeriodRewardInfo ---------------------------------------------------

// ORIG 0x08a00c70：三个 string（+0x4/+0x10/+0x14）默认构造 → 标量清零
// （word@0、int@8、int@c）→ 三个 string 依次赋 ""（ORIG 空串常量 @0x08e1ce20）。
stFavorableRelationShip_t::stPeriodRewardInfo::stPeriodRewardInfo()
{
    m_type = 0;
    static const char* const kEmptyString = "";
    m_name = kEmptyString;
    m_a = 0;
    m_b = 0;
    m_desc = kEmptyString;
    m_param = kEmptyString;
}

// ORIG 0x085819e4（W 0x73）：空体（三 string 成员按逆序 0x14→0x10→0x4 自动析构）。
stFavorableRelationShip_t::stPeriodRewardInfo::~stPeriodRewardInfo()
{
}


