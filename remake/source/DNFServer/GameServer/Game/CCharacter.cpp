// ============================================================================
// df_game_r 还原 —— CCharacter（依据 docs/class_func_reports/CCharacter/ 报告）。
// ctor ORIG _ZN10CCharacterC1Ev T 0x0834828c / dtor D1 T 0x0834842a。
// ============================================================================
#include "CCharacter.h"

// ORIG STSecondGrowTypeInfo::STSecondGrowTypeInfo W 0x8373934：
// 2 个 _Additioal_info 构造 + 2 个 vector 默认构造。
STSecondGrowTypeInfo::STSecondGrowTypeInfo()
    : m_addInfo(), m_skills(), m_fieldbc(0)
{
}

// ORIG ~STSecondGrowTypeInfo W 0x8376324：逆序析构 +0xa4/+0xb0 两个 vector。
STSecondGrowTypeInfo::~STSecondGrowTypeInfo()
{
}

// ORIG CCharacter::CCharacter @ 0x834828c：
//   _Additioal_info[7]（this+0 起，步长 0x52）
//   vector[7]（+0x240，步长 0xc）
//   STSecondGrowTypeInfo[7]（+0x294，步长 0xc0）→ clear()。
CCharacter::CCharacter()
    : m_baseInfo(), m_giveSkillList(), m_secondGrowTypes()
{
    // ORIG 0x834828c：各成员构造完成后调 clear()。
    clear();
}

// ORIG ~CCharacter @ 0x834842a：destroy → 逆序析构 STSecondGrowTypeInfo[7]、
// vector[7]（成员析构由编译器按声明逆序生成，语义一致）。
CCharacter::~CCharacter()
{
    // ORIG ~CCharacter @ 0x834842a：先调 destroy()（→clear()）再析构成员。
    destroy();
}

// ORIG get_give_skill_firstGrowtype @ 0x83485d8。
// ORIG 地址 = this + 0x240 + 12*(grow+1)（set_data 填满 [0..6]，本查询跳过 0 号基础槽）。
char CCharacter::get_give_skill_firstGrowtype(int grow,
                                              std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    std::vector<std::pair<int, int> >::const_iterator it = m_giveSkillList[grow + 1].begin();
    std::vector<std::pair<int, int> >::const_iterator e = m_giveSkillList[grow + 1].end();
    for (; it != e; ++it)
    {
        const std::pair<int, int>& p = *it;
        out.push_back(p);
    }
    return 1;
}

// ORIG get_give_skill_secondGrowtype @ 0x8348694。
// 注意：secondGrow 越界（<1 或 >2）时返回 1 且不拷贝（ORIG uVar2=1 分支）。
// ORIG 地址 = this + 0x330 + 0xc0*(grow+1) + 12*(secondGrow-1) + 8
//          = &m_secondGrowTypes[grow+1].m_skills[secondGrow-1]（同样跳过 0 号基础槽）。
char CCharacter::get_give_skill_secondGrowtype(int grow, int secondGrow,
                                               std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    if (secondGrow < 1 || secondGrow > 2)
        return 1;
    std::vector<std::pair<int, int> >::const_iterator it =
        m_secondGrowTypes[grow + 1].m_skills[secondGrow - 1].begin();
    std::vector<std::pair<int, int> >::const_iterator e =
        m_secondGrowTypes[grow + 1].m_skills[secondGrow - 1].end();
    for (; it != e; ++it)
    {
        const std::pair<int, int>& p = *it;
        out.push_back(p);
    }
    return 1;
}

// ORIG get_growtype_stat @ 0x834883e：按 (firstGrow, secondGrow) 返回成长属性块
// 指针；越界（firstGrow>5 或 secondGrow>2）返回 NULL。secondGrow==0 时取
// firstGrow*0x52+0x52，否则取 (secondGrow-1)*0x52+(firstGrow+1)*0xc0+0x294。
const void* CCharacter::get_growtype_stat(int firstGrow, int secondGrow) const
{
    if (firstGrow > 5 || secondGrow > 2)
        return 0;
    if (secondGrow == 0)
        return (const char*)this + firstGrow * 0x52 + 0x52;
    return (const char*)this + (secondGrow - 1) * 0x52 +
           (firstGrow + 1) * 0xc0 + 0x294;
}

// ORIG get_give_skill @ 0x8348798。
char CCharacter::get_give_skill(int firstGrow, int secondGrow,
                                std::vector<std::pair<int, int> >& out,
                                unsigned int flag) const
{
    if (flag == 0)
    {
        char r = get_give_skill_firstGrowtype(firstGrow, out);
        r = (char)get_give_skill_secondGrowtype(firstGrow, secondGrow, out);
        return r;
    }
    if (flag == 1)
        return get_give_skill_firstGrowtype(firstGrow, out);
    if (flag == 2)
        return get_give_skill_secondGrowtype(firstGrow, secondGrow, out);
    return 0;
}

// ============================================================================
// 以下为 CharacterAIA2 批次追加（报告 docs/class_func_reports/CCharacter/
// {CCharacter.md,~CCharacter.md} 完整转写；destroy/set_data/clear 报告缺失
// → objdump 转写，ORIG nm：destroy T 0x083485c4 / clear T 0x0834852e /
//   set_data T 0x08348894）。
// ============================================================================
#include "STCharacterScript.h"

// ORIG STSecondGrowTypeInfo::clear W 0x08373a1e（objdump 转写）：
// i=0..1 { m_addInfo[i].clear(); m_skills[i].clear(); } 后 memset(&m_fieldbc,0,2)
// —— 仅清 m_fieldbc 低 2 字节，高 2 字节保留（ORIG 固有）。
void STSecondGrowTypeInfo::clear()
{
    for (int i = 0; i <= 1; ++i)
    {
        m_addInfo[i].clear();
        m_skills[i].clear();
    }
    memset(&m_fieldbc, 0, 2);
}

// ORIG CCharacter::destroy @ 0x083485c4：sub $0x18 帧 + call clear()。
void CCharacter::destroy()
{
    clear();
}

// ORIG CCharacter::clear @ 0x0834852e：
//   m_baseInfo[0..6] 逐个 _Additioal_info::clear；
//   循环 i=0..6 { m_giveSkillList[i].clear(); m_secondGrowTypes[i].clear(); }。
void CCharacter::clear()
{
    for (int i = 0; i < 7; ++i)
        m_baseInfo[i].clear();
    for (int i = 0; i <= 6; ++i)
    {
        m_giveSkillList[i].clear();
        m_secondGrowTypes[i].clear();
    }
}

// ORIG CCharacter::set_data @ 0x08348894（objdump 全量转写）。
// 源表布局（STCharacterScript.h 权威）：行 i=0 取 m_initialStatus(+0x24)，
// 行 i=1..6 取 m_growStatus[0..5]；每行 STRDCharacterStatus(0x57 packed) 字段与
// _Additioal_info(0x52 packed) 一一同名对应；尾字节 +0x76(m_field52) 写
// m_growTypeFlag[i]。随后 m_giveSkillList[i] = script->m_skillList[i]。
// 第二成长域：标量走具名字段（dst=m_secondGrowTypes[a].m_addInfo[b]，源=
// m_secondGrow[a].m_statusA/B）；resist 两段循环在 ORIG 中以裸指针算术寻址，
// B 维度步长 0x50 与成员布局 0x52 存在固有 -2B 偏移（B=1 时落在 addInfo 内
// 偏移 -2 处）——按 ORIG 指令逐字节镜像，不改为具名字段访问。
void CCharacter::set_data(STCharacterScript* script)
{
    const STRDCharacterStatus* rows[7];
    rows[0] = &script->m_initialStatus;
    for (int i = 1; i <= 6; ++i)
        rows[i] = &script->m_growStatus[i - 1];

    for (int i = 0; i <= 6; ++i)
    {
        _Additioal_info& dst = m_baseInfo[i];
        const STRDCharacterStatus& s = *rows[i];
        dst.m_field00 = s.m_hpMax;
        dst.m_field04 = s.m_mpMax;
        dst.m_field08 = s.m_physAttack;
        dst.m_field0a = s.m_physDefense;
        dst.m_field0c = s.m_magAttack;
        dst.m_field0e = s.m_magDefense;
        for (int j = 0; j <= 3; ++j)
            dst.m_field10[j] = s.m_elemResist[j];
        for (int k = 0; k <= 16; ++k)
            dst.m_field18[k] = s.m_activeResist[k];
        dst.m_field3a = s.m_inventoryLimit;
        dst.m_field3e = s.m_hpRegen;
        dst.m_field40 = s.m_mpRegen;
        dst.m_field42 = s.m_moveSpeed;
        dst.m_field46 = s.m_attackSpeed;
        dst.m_field48 = s.m_castSpeed;
        dst.m_field4a = s.m_hitRecovery;
        dst.m_field4c = s.m_jumpPower;
        dst.m_field4e = s.m_weight;
        m_growTypeFlag[i] = (char)s.m_field52;
        m_giveSkillList[i] = script->m_skillList[i];   // vector::operator=
    }

    for (int a = 0; a <= 6; ++a)
    {
        for (int b = 0; b <= 1; ++b)
        {
            const STRDCharacterStatus& st = (b == 0)
                ? script->m_secondGrow[a].m_statusA
                : script->m_secondGrow[a].m_statusB;
            _Additioal_info& dst = m_secondGrowTypes[a].m_addInfo[b];
            dst.m_field00 = st.m_hpMax;
            dst.m_field04 = st.m_mpMax;
            dst.m_field08 = st.m_physAttack;
            dst.m_field0a = st.m_physDefense;
            dst.m_field0c = st.m_magAttack;
            dst.m_field0e = st.m_magDefense;
            dst.m_field3a = st.m_inventoryLimit;
            dst.m_field3e = st.m_hpRegen;
            dst.m_field40 = st.m_mpRegen;
            dst.m_field42 = st.m_moveSpeed;
            dst.m_field46 = st.m_attackSpeed;
            dst.m_field48 = st.m_castSpeed;
            dst.m_field4a = st.m_hitRecovery;
            dst.m_field4c = st.m_jumpPower;
            dst.m_field4e = st.m_weight;

            // ORIG loop-t EA = this + (b*0x28 + a*0x60 + t + 0x150)*2 + 4
            unsigned short* pElem = reinterpret_cast<unsigned short*>(
                reinterpret_cast<char*>(this) +
                (b * 0x28 + a * 0x60 + 0x150) * 2 + 4);
            for (int t = 0; t <= 3; ++t)
                pElem[t] = st.m_elemResist[t];
            // ORIG loop-k EA = this + (b*0x28 + a*0x60 + k + 0x150)*2 + 0xc
            unsigned short* pAct = reinterpret_cast<unsigned short*>(
                reinterpret_cast<char*>(this) +
                (b * 0x28 + a * 0x60 + 0x150) * 2 + 0xc);
            for (int k = 0; k <= 16; ++k)
                pAct[k] = st.m_activeResist[k];
        }
    }
}
