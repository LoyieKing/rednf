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
}

// ORIG ~CCharacter @ 0x834842a：destroy → 逆序析构 STSecondGrowTypeInfo[7]、
// vector[7]（成员析构由编译器按声明逆序生成，语义一致）。
CCharacter::~CCharacter()
{
}

// ORIG get_give_skill_firstGrowtype @ 0x83485d8。
char CCharacter::get_give_skill_firstGrowtype_impl(int grow,
                                                   std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    const std::vector<std::pair<int, int> >& src = m_giveSkillList[grow];
    for (std::vector<std::pair<int, int> >::const_iterator it = src.begin();
         it != src.end(); ++it)
        out.push_back(*it);
    return 1;
}

// ORIG get_give_skill_secondGrowtype @ 0x8348694。
// 注意：secondGrow 越界（<1 或 >2）时返回 1 且不拷贝（ORIG uVar2=1 分支）。
char CCharacter::get_give_skill_secondGrowtype_impl(int grow, int secondGrow,
                                                    std::vector<std::pair<int, int> >& out) const
{
    if (grow < 0 || grow > 5)
        return 0;
    if (secondGrow < 1 || secondGrow > 2)
        return 1;
    const std::vector<std::pair<int, int> >& src = m_secondGrowTypes[grow].m_skills[secondGrow - 1];
    for (std::vector<std::pair<int, int> >::const_iterator it = src.begin();
         it != src.end(); ++it)
        out.push_back(*it);
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
        char r = get_give_skill_firstGrowtype_impl(firstGrow, out);
        r = (char)get_give_skill_secondGrowtype_impl(firstGrow, secondGrow, out);
        return r;
    }
    if (flag == 1)
        return get_give_skill_firstGrowtype_impl(firstGrow, out);
    if (flag == 2)
        return get_give_skill_secondGrowtype_impl(firstGrow, secondGrow, out);
    return 0;
}
