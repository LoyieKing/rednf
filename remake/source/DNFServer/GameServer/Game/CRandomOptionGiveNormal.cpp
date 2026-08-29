// ORIG 翻译：CRandomOptionGiveNormal（见 CRandomOptionGiveNormal.h 头注释）
// 5 符号：C1/C2、get_option_type、get_option_group_choice、set。
// 派生 override 直返指针（引用 ABI = 地址），set 依 ORIG 序存两成员。
#include "CRandomOptionGiveNormal.h"

CRandomOptionGiveNormal::CRandomOptionGiveNormal()
    : m_pMapType4(0), m_pMapChoice8(0)
{
}

std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >&
CRandomOptionGiveNormal::get_option_type()
{
    return *m_pMapType4;
}

std::map<unsigned long long, OptionGroupChoiceValue>&
CRandomOptionGiveNormal::get_option_group_choice()
{
    return *m_pMapChoice8;
}

void CRandomOptionGiveNormal::set(
    std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >* typeMap,
    std::map<unsigned long long, OptionGroupChoiceValue>* choiceMap)
{
    m_pMapType4 = typeMap;
    m_pMapChoice8 = choiceMap;
}
